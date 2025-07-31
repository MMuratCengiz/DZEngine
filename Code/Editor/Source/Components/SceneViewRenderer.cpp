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

#include "DZEngine/Components/SceneViewRenderer.h"
#include <array>
#include "DenOfIzGraphics/Assets/Import/ShaderImporter.h"
#include "DenOfIzGraphics/Data/BatchResourceCopy.h"
#include "DenOfIzGraphics/Utilities/InteropUtilities.h"

using namespace DZEngine;
using namespace DenOfIz;

SceneViewRenderer::SceneViewRenderer( const SceneViewRendererDesc &desc ) :
    m_logicalDevice( desc.LogicalDevice ), m_viewport( desc.Viewport ), m_numFramesInFlight( desc.NumFramesInFlight )
{
    CreateVertexBuffer( );
    CreateShaderProgram( );
    CreatePipeline( );
    CreateRenderTargets( );
}

void SceneViewRenderer::UpdateViewport( const Viewport &viewport )
{
    if ( m_viewport.Width != viewport.Width || m_viewport.Height != viewport.Height )
    {
        m_viewport = viewport;
        CreateRenderTargets( );
    }
}

void SceneViewRenderer::Render( ICommandList *commandList, ITextureResource *renderTarget, uint32_t frameIndex )
{
    if ( !m_pipeline || !renderTarget )
    {
        return;
    }

    ITextureResource *targetTexture = renderTarget ? renderTarget : GetRenderTexture( frameIndex );

    m_resourceTracking.TransitionTexture( commandList, targetTexture, ResourceUsage::RenderTarget );

    RenderingAttachmentDesc attachmentDesc{ };
    attachmentDesc.Resource = targetTexture;

    RenderingDesc renderingDesc{ };
    renderingDesc.RTAttachments.Elements    = &attachmentDesc;
    renderingDesc.RTAttachments.NumElements = 1;
    commandList->BeginRendering( renderingDesc );

    commandList->BindViewport( m_viewport.X, m_viewport.Y, m_viewport.Width, m_viewport.Height );
    commandList->BindScissorRect( m_viewport.X, m_viewport.Y, m_viewport.Width, m_viewport.Height );
    commandList->BindPipeline( m_pipeline.get( ) );
    commandList->BindVertexBuffer( m_vertexBuffer.get( ) );
    commandList->Draw( 3, 1, 0, 0 );

    commandList->EndRendering( );

    m_resourceTracking.TransitionTexture( commandList, targetTexture, ResourceUsage::ShaderResource );
}

ITextureResource *SceneViewRenderer::GetRenderTexture( uint32_t frameIndex ) const
{
    if ( frameIndex < m_renderTargets.size( ) && m_renderTargets[ frameIndex ] )
    {
        return m_renderTargets[ frameIndex ].get( );
    }
    return nullptr;
}

void SceneViewRenderer::CreateVertexBuffer( )
{
    constexpr std::array vertices = { 0.0f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f };

    BufferDesc bufferDesc{ };
    bufferDesc.Descriptor = ResourceDescriptor::VertexBuffer;
    bufferDesc.NumBytes   = vertices.size( ) * sizeof( float );
    bufferDesc.DebugName  = "SceneViewTriangleVertexBuffer";

    m_vertexBuffer = std::unique_ptr<IBufferResource>( m_logicalDevice->CreateBufferResource( bufferDesc ) );

    BatchResourceCopy batchCopy( m_logicalDevice );
    batchCopy.Begin( );

    CopyToGpuBufferDesc copyDesc{ };
    copyDesc.DstBuffer        = m_vertexBuffer.get( );
    copyDesc.Data.Elements    = reinterpret_cast<const Byte *>( &vertices[ 0 ] );
    copyDesc.Data.NumElements = sizeof( vertices );
    batchCopy.CopyToGPUBuffer( copyDesc );
    batchCopy.Submit( );

    m_resourceTracking.TrackBuffer( m_vertexBuffer.get( ), ResourceUsage::VertexAndConstantBuffer );
}

void SceneViewRenderer::CreateShaderProgram( )
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

void SceneViewRenderer::CreatePipeline( )
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

void SceneViewRenderer::CreateRenderTargets( )
{
    if ( m_viewport.Width <= 0 || m_viewport.Height <= 0 )
    {
        return;
    }

    m_renderTargets.clear( );
    m_depthTextures.clear( );

    for ( uint32_t i = 0; i < m_numFramesInFlight; ++i )
    {
        TextureDesc renderTargetDesc{ };
        renderTargetDesc.Width      = static_cast<uint32_t>( m_viewport.Width );
        renderTargetDesc.Height     = static_cast<uint32_t>( m_viewport.Height );
        renderTargetDesc.Format     = Format::B8G8R8A8Unorm;
        renderTargetDesc.Descriptor = ResourceDescriptor::RenderTarget | ResourceDescriptor::Texture;
        renderTargetDesc.DebugName  = "SceneViewRenderTarget";

        m_renderTargets.push_back( std::unique_ptr<ITextureResource>( m_logicalDevice->CreateTextureResource( renderTargetDesc ) ) );

        m_resourceTracking.TrackTexture( m_renderTargets[ i ].get( ), ResourceUsage::ShaderResource );

        TextureDesc depthDesc{ };
        depthDesc.Width      = static_cast<uint32_t>( m_viewport.Width );
        depthDesc.Height     = static_cast<uint32_t>( m_viewport.Height );
        depthDesc.Format     = Format::D32Float;
        depthDesc.Descriptor = ResourceDescriptor::DepthStencil;
        depthDesc.Usages     = ResourceUsage::DepthWrite;
        depthDesc.DebugName  = "SceneViewDepthBuffer";

        m_depthTextures.push_back( std::unique_ptr<ITextureResource>( m_logicalDevice->CreateTextureResource( depthDesc ) ) );
        m_resourceTracking.TrackTexture( m_depthTextures[ i ].get( ), ResourceUsage::DepthWrite );
    }
}

ByteArray SceneViewRenderer::GetVertexShader( ) const
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

ByteArray SceneViewRenderer::GetPixelShader( ) const
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
