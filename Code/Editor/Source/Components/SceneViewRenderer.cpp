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
#include <spdlog/spdlog.h>

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

void SceneViewRenderer::Render( ICommandList *commandList, const uint32_t frameIndex )
{
    ITextureResource *renderTarget = GetRenderTarget( frameIndex );

    m_resourceTracking.TransitionTexture( commandList, renderTarget, ResourceUsage::RenderTarget );

    RenderingAttachmentDesc attachmentDesc{ };
    attachmentDesc.Resource = renderTarget;

    RenderingDesc renderingDesc{ };
    renderingDesc.RTAttachments.Elements    = &attachmentDesc;
    renderingDesc.RTAttachments.NumElements = 1;
    commandList->BeginRendering( renderingDesc );

    commandList->BindViewport( m_viewport.X, m_viewport.Y, m_viewport.Width, m_viewport.Height );
    commandList->BindScissorRect( m_viewport.X, m_viewport.Y, m_viewport.Width, m_viewport.Height );
    commandList->BindPipeline( m_pipeline.get( ) );
    commandList->BindVertexBuffer( m_meshBatch->GetVertexBuffer( ).Buffer );
    commandList->BindIndexBuffer( m_meshBatch->GetIndexBuffer( ).Buffer, IndexType::Uint32 );
    commandList->DrawIndexed( 36, 1, 0, 0 );

    commandList->EndRendering( );

    m_resourceTracking.TransitionTexture( commandList, renderTarget, ResourceUsage::ShaderResource );
}

ITextureResource *SceneViewRenderer::GetRenderTarget( const uint32_t frameIndex ) const
{
    if ( frameIndex < m_renderTargets.size( ) && m_renderTargets[ frameIndex ] )
    {
        return m_renderTargets[ frameIndex ].get( );
    }
    spdlog::error( "Invalid frame index" );
    return nullptr;
}

void SceneViewRenderer::CreateVertexBuffer( )
{
    MeshPoolDesc meshPoolDesc{ };
    meshPoolDesc.LogicalDevice = m_logicalDevice;
    m_meshBatch                = std::make_unique<MeshBatch>( meshPoolDesc );
    m_meshBatch->BeginUpdate( );

    BoxDesc boxDesc{ };
    boxDesc.BuildDesc = 0;
    boxDesc.Width     = 1.0f;
    boxDesc.Height    = 1.0f;
    boxDesc.Depth     = 1.0f;

    const auto box = std::unique_ptr<GeometryData>( Geometry::BuildBox( boxDesc ) );
    m_meshBatch->AddGeometry( box.get( ) );

    SphereDesc sphereDesc{ };
    sphereDesc.BuildDesc    = 0;
    sphereDesc.Diameter     = 1.0f;
    sphereDesc.Tessellation = 24.0f;
    const auto sphere       = std::unique_ptr<GeometryData>( Geometry::BuildSphere( sphereDesc ) );
    m_meshBatch->AddGeometry( sphere.get( ) );
    m_meshBatch->EndUpdate( );
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
    m_shaderProgram                            = std::make_unique<ShaderProgram>( shaderProgramDesc );

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
            float4 Position : POSITION;
            float4 Normal : NORMAL;
            float2 TexCoord : TEXCOORD0;
            float4 Color : COLOR0;
            float4 Tangent : TANGENT0;
        };

        struct PSInput
        {
            float4 Position : SV_POSITION;
            float4 Color : COLOR;
        };

        PSInput VSMain(VSInput input)
        {
            PSInput output;
            output.Position = input.Position;
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
