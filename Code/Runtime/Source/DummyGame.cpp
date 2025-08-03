/*
Den Of Iz - Game/Game Engine
Copyright (c) 2020-2024 Muhammed Murat Cengiz

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "DZEngine/DummyGame.h"
#include <array>
#include "DenOfIzGraphics/Assets/Import/ShaderImporter.h"
#include "DenOfIzGraphics/Data/BatchResourceCopy.h"
#include "DenOfIzGraphics/Utilities/InteropUtilities.h"

using namespace DZEngine;
using namespace DenOfIz;

void DummyGame::Init( AppContext *appContext )
{
    m_appContext       = appContext;
    m_logicalDevice    = appContext->GraphicsContext->LogicalDevice;
    m_resourceTracking = appContext->GraphicsContext->ResourceTracking;

    // Scene* scene = m_appContext->World->CreateScene( "Playground" );

    CreateVertexBuffer( );
    CreateShaderProgram( );
    CreatePipeline( );

    CommandListPoolDesc commandListPoolDesc{ };
    commandListPoolDesc.CommandQueue    = m_appContext->GraphicsContext->GraphicsQueue;
    commandListPoolDesc.NumCommandLists = m_appContext->GraphicsContext->NumFramesInFlight;
    m_commandListPool                   = std::unique_ptr<ICommandListPool>( m_logicalDevice->CreateCommandListPool( commandListPoolDesc ) );

    const ICommandListArray commandLists = m_commandListPool->GetCommandLists( );
    m_commandLists.resize( commandLists.NumElements );
    for ( uint32_t i = 0; i < commandLists.NumElements; ++i )
    {
        m_commandLists[ i ] = commandLists.Elements[ i ];
    }
}

DummyGame::~DummyGame( )
{
}

void DummyGame::HandleEvent( const Event &event )
{
}

void DummyGame::Update( )
{
}

bool DummyGame::Render( RenderDesc renderDesc )
{
    if ( !m_pipeline || !renderDesc.RenderTarget || renderDesc.FrameIndex >= m_commandLists.size( ) )
    {
        return false;
    }

    ICommandList *commandList = m_commandLists[ renderDesc.FrameIndex ];

    commandList->Begin( );

    m_resourceTracking->TransitionTexture( commandList, renderDesc.RenderTarget, ResourceUsage::RenderTarget );

    RenderingAttachmentDesc attachmentDesc{ };
    attachmentDesc.Resource = renderDesc.RenderTarget;

    RenderingDesc renderingDesc{ };
    renderingDesc.RTAttachments.Elements    = &attachmentDesc;
    renderingDesc.RTAttachments.NumElements = 1;
    commandList->BeginRendering( renderingDesc );

    commandList->BindViewport( renderDesc.Viewport.X, renderDesc.Viewport.Y, renderDesc.Viewport.Width, renderDesc.Viewport.Height );
    commandList->BindScissorRect( renderDesc.Viewport.X, renderDesc.Viewport.Y, renderDesc.Viewport.Width, renderDesc.Viewport.Height );
    commandList->BindPipeline( m_pipeline.get( ) );
    commandList->BindVertexBuffer( m_vertexBuffer.get( ) );
    commandList->Draw( 3, 1, 0, 0 );

    commandList->EndRendering( );

    m_resourceTracking->TransitionTexture( commandList, renderDesc.RenderTarget, ResourceUsage::Present );

    commandList->End( );

    ExecuteCommandListsDesc executeDesc{ };
    executeDesc.CommandLists.Elements    = &commandList;
    executeDesc.CommandLists.NumElements = 1;
    if ( renderDesc.NotifyFence )
    {
        executeDesc.Signal = renderDesc.NotifyFence;
    }
    if ( renderDesc.SignalSemaphore )
    {
        executeDesc.SignalSemaphores.Elements    = &renderDesc.SignalSemaphore;
        executeDesc.SignalSemaphores.NumElements = 1;
    }
    m_appContext->GraphicsContext->GraphicsQueue->ExecuteCommandLists( executeDesc );

    return true;
}

void DummyGame::CreateVertexBuffer( )
{
    constexpr std::array vertices = { 0.0f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f };

    BufferDesc bufferDesc{ };
    bufferDesc.Descriptor = ResourceDescriptor::VertexBuffer;
    bufferDesc.NumBytes   = vertices.size( ) * sizeof( float );
    bufferDesc.DebugName  = "GameViewTriangleVertexBuffer";

    m_vertexBuffer = std::unique_ptr<IBufferResource>( m_logicalDevice->CreateBufferResource( bufferDesc ) );

    BatchResourceCopy batchCopy( m_logicalDevice );
    batchCopy.Begin( );

    CopyToGpuBufferDesc copyDesc{ };
    copyDesc.DstBuffer        = m_vertexBuffer.get( );
    copyDesc.Data.Elements    = reinterpret_cast<const Byte *>( &vertices[ 0 ] );
    copyDesc.Data.NumElements = sizeof( vertices );
    batchCopy.CopyToGPUBuffer( copyDesc );
    batchCopy.Submit( );

    m_resourceTracking->TrackBuffer( m_vertexBuffer.get( ), ResourceUsage::VertexAndConstantBuffer );
}

void DummyGame::CreateShaderProgram( )
{
    std::array<ShaderStageDesc, 2> shaderStages{ };

    ShaderStageDesc &vertexShaderDesc = shaderStages[ 0 ];
    vertexShaderDesc.Stage            = ShaderStage::Vertex;
    vertexShaderDesc.EntryPoint       = "VSMain";
    vertexShaderDesc.Data             = GetVertexShader( );

    ShaderStageDesc &pixelShaderDesc = shaderStages[ 1 ];
    pixelShaderDesc.Stage            = ShaderStage::Pixel;
    pixelShaderDesc.EntryPoint       = "PSMain";
    pixelShaderDesc.Data             = GetPixelShader( );

    ShaderProgramDesc shaderProgramDesc{ };
    shaderProgramDesc.ShaderStages.Elements    = shaderStages.data( );
    shaderProgramDesc.ShaderStages.NumElements = shaderStages.size( );
    m_shaderProgram                            = std::unique_ptr<ShaderProgram>( ShaderImporter::ImportCached( shaderProgramDesc ) );

    std::free( vertexShaderDesc.Data.Elements );
    std::free( pixelShaderDesc.Data.Elements );
}

void DummyGame::CreatePipeline( )
{
    const ShaderReflectDesc reflectDesc = m_shaderProgram->Reflect( );
    m_inputLayout                       = std::unique_ptr<IInputLayout>( m_logicalDevice->CreateInputLayout( reflectDesc.InputLayout ) );
    m_rootSignature                     = std::unique_ptr<IRootSignature>( m_logicalDevice->CreateRootSignature( reflectDesc.RootSignature ) );

    PipelineDesc pipelineDesc{ };
    pipelineDesc.InputLayout   = m_inputLayout.get( );
    pipelineDesc.ShaderProgram = m_shaderProgram.get( );
    pipelineDesc.RootSignature = m_rootSignature.get( );

    RenderTargetDesc renderTargetDesc{ };
    renderTargetDesc.Format                         = Format::B8G8R8A8Unorm;
    pipelineDesc.Graphics.RenderTargets.NumElements = 1;
    pipelineDesc.Graphics.RenderTargets.Elements    = &renderTargetDesc;

    m_pipeline = std::unique_ptr<IPipeline>( m_logicalDevice->CreatePipeline( pipelineDesc ) );
}

ByteArray DummyGame::GetVertexShader( ) const
{
    const auto shaderCode = R"(
        struct VSInput
        {
            float3 Position : POSITION;
            float4 Color : COLOR;
        };

        struct PSInput
        {
            float4 Position : SV_POSITION;
            float4 Color : COLOR;
        };

        PSInput VSMain(VSInput input)
        {
            PSInput output;
            output.Position = float4(input.Position, 1.0);
            output.Color = input.Color;
            return output;
        }
        )";

    return InteropUtilities::StringToBytes( shaderCode );
}

ByteArray DummyGame::GetPixelShader( ) const
{
    const auto shaderCode = R"(
        struct PSInput
        {
            float4 Position : SV_POSITION;
            float4 Color : COLOR;
        };

        float4 PSMain(PSInput input) : SV_TARGET
        {
            return input.Color;
        }
        )";

    return InteropUtilities::StringToBytes( shaderCode );
}
