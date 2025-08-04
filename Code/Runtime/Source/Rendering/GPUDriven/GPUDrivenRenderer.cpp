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

#include "DZEngine/Rendering/GPUDriven/GPUDrivenRenderer.h"
#include "DZEngine/Rendering/GPUDriven/GPUDrivenBinding.h"

using namespace DZEngine;

GPUDrivenRenderer::GPUDrivenRenderer( const RendererDesc &rendererDesc )
{
    m_graphicsContext = rendererDesc.AppContext->GraphicsContext;
    m_numFrames       = rendererDesc.AppContext->NumFrames;
    m_assetBatcher    = rendererDesc.AppContext->AssetBatcher;
    m_world           = rendererDesc.AppContext->World;

    m_rootSig = std::make_unique<GPUDrivenRootSig>( m_graphicsContext->LogicalDevice );

    m_batches.resize( m_assetBatcher->NumBatches( ) );

    for ( int i = 0; i < m_assetBatcher->NumBatches( ); ++i )
    {
        GPUDrivenDataUploadDesc uploadDesc{ };
        uploadDesc.GraphicsContext = m_graphicsContext;
        uploadDesc.Assets          = m_assetBatcher;
        uploadDesc.World           = m_world;
        uploadDesc.BatchId         = i;
        uploadDesc.NumFrames       = rendererDesc.AppContext->NumFrames;
        m_batches[ i ]->DataUpload = std::make_unique<GPUDrivenDataUpload>( uploadDesc );

        GPUDrivenBindingDesc bindingDesc{ };
        bindingDesc.RootSig         = m_rootSig.get( );
        bindingDesc.DataUpload      = m_batches[ i ]->DataUpload.get( );
        bindingDesc.AppContext      = rendererDesc.AppContext;
        bindingDesc.BatchId         = i;
        m_batches[ i ]->DataBinding = std::make_unique<GPUDrivenBinding>( bindingDesc );
    }

    InitTestPipeline( );
}

void GPUDrivenRenderer::RenderFrame( const RenderFrameDesc &renderFrame )
{
    for ( int i = 0; i < m_assetBatcher->NumBatches( ); ++i )
    {
        const auto &binding = m_batches[ i ]->DataBinding;
        binding->Update( renderFrame.FrameIndex );
    }

    const auto cmdList = m_commandLists[ renderFrame.FrameIndex ];
    cmdList->Begin( );

    for ( int i = 0; i < m_assetBatcher->NumBatches( ); ++i )
    {
        std::array<RenderingAttachmentDesc, 1> attachments{ }; // Todo add depth
        attachments[ 0 ].Resource = renderFrame.RenderTarget;

        RenderingDesc renderingDesc{ };
        renderingDesc.RTAttachments.Elements    = attachments.data( );
        renderingDesc.RTAttachments.NumElements = attachments.size( );

        const Viewport &vp = renderFrame.Viewport;

        cmdList->BeginRendering( renderingDesc );
        cmdList->BindViewport( vp.X, vp.Y, vp.Width, vp.Height );
        cmdList->BindScissorRect( vp.X, vp.Y, vp.Width, vp.Height );
        cmdList->BindPipeline( m_pipeline.get( ) );

        const auto &binding = m_batches[ i ]->DataBinding;

        cmdList->BindResourceGroup( binding->GetSamplerBinding( ) );
        cmdList->BindResourceGroup( binding->GetBuffersBinding( renderFrame.FrameIndex ) );
        cmdList->BindResourceGroup( binding->GetTexturesBinding( renderFrame.FrameIndex ) );

        const auto buffers = m_batches[ i ]->DataUpload->GetBuffers( renderFrame.FrameIndex );
        const uint32_t drawCount = m_batches[ i ]->DataUpload->GetNumDraws( renderFrame.FrameIndex );
        
        if ( drawCount > 0 )
        {
            cmdList->DrawIndexedIndirect( buffers.IndirectBuffer, 0, drawCount, sizeof( DrawIndexedIndirectCommand ) );
        }

        cmdList->EndRendering( );
    }

    cmdList->End( );
}

void GPUDrivenRenderer::InitTestPipeline( )
{
    std::array<ShaderStageDesc, 2> shaderStages{ };

    ShaderStageDesc vertShaderStageDesc{ };
    vertShaderStageDesc.Stage = ShaderStage::Vertex;
    vertShaderStageDesc.Path  = "_Assets/Engine/Shaders/GPUDriven/UberShader.vs.hlsl";
    shaderStages[ 0 ]         = vertShaderStageDesc;

    ShaderStageDesc pixShaderStageDesc{ };
    pixShaderStageDesc.Stage = ShaderStage::Pixel;
    pixShaderStageDesc.Path  = "_Assets/Engine/Shaders/GPUDriven/UberShader.ps.hlsl";
    shaderStages[ 1 ]        = pixShaderStageDesc;

    ShaderProgramDesc shaderProgramDesc{ };
    shaderProgramDesc.ShaderStages.Elements    = shaderStages.data( );
    shaderProgramDesc.ShaderStages.NumElements = shaderStages.size( );
    m_program                                  = std::make_unique<ShaderProgram>( shaderProgramDesc );

    CommandQueueDesc cmdQueueDesc{ };
    cmdQueueDesc.QueueType = QueueType::Graphics;

    m_commandQueue = std::unique_ptr<ICommandQueue>( m_graphicsContext->LogicalDevice->CreateCommandQueue( cmdQueueDesc ) );

    CommandListPoolDesc cmdListPoolDesc{ };
    cmdListPoolDesc.NumCommandLists = 3;
    cmdListPoolDesc.CommandQueue    = m_commandQueue.get( );
    m_commandListPool               = std::unique_ptr<ICommandListPool>( m_graphicsContext->LogicalDevice->CreateCommandListPool( cmdListPoolDesc ) );

    const auto commandListArray = m_commandListPool->GetCommandLists( );
    m_commandLists.resize( commandListArray.NumElements );
    for ( int i = 0; i < commandListArray.NumElements; ++i )
    {
        m_commandLists[ i ] = commandListArray.Elements[ i ];
    }

    PipelineDesc pipelineDesc{ };
    pipelineDesc.RootSignature = m_rootSig->GetRootSignature( );
    pipelineDesc.ShaderProgram = m_program.get( );
    pipelineDesc.BindPoint     = BindPoint::Graphics;

    RenderTargetDesc renderTargetDesc{ };
    renderTargetDesc.Format = Format::B8G8R8A8Unorm;

    pipelineDesc.Graphics.RenderTargets.Elements    = &renderTargetDesc;
    pipelineDesc.Graphics.RenderTargets.NumElements = 1;

    m_pipeline = std::unique_ptr<IPipeline>( m_graphicsContext->LogicalDevice->CreatePipeline( pipelineDesc ) );
}
