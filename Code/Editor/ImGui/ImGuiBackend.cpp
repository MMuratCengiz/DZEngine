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

#include "ImGuiBackend.h"

#include <spdlog/spdlog.h>

#include "DenOfIzGraphics/Data/BatchResourceCopy.h"
#include "DenOfIzGraphics/Support/ResourceTracking.h"

using namespace DenOfIz;

namespace
{
    auto ImGuiVertexShaderSource = R"(
struct VSInput
{
    float2 Position : POSITION;
    float2 TexCoord : TEXCOORD0;
    uint   Color : COLOR0;
};

struct VSOutput
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
    float4 Color : COLOR0;
};

cbuffer ImGuiUniforms : register(b0, space1)
{
    float4x4 Projection;
    float4 ScreenSize;
};

VSOutput main(VSInput input)
{
    VSOutput output;
    output.Position = mul(float4(input.Position, 0.0, 1.0), Projection);
    output.TexCoord = input.TexCoord;
    output.Color = float4(
        ((input.Color >> 0) & 0xFF) / 255.0f,
        ((input.Color >> 8) & 0xFF) / 255.0f,
        ((input.Color >> 16) & 0xFF) / 255.0f,
        ((input.Color >> 24) & 0xFF) / 255.0f
    );
    return output;
})";

    auto ImGuiPixelShaderSource = R"(
struct PSInput
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
    float4 Color : COLOR0;
};

cbuffer PixelConstants : register(b0, space2)
{
    uint TextureIndex;
};

Texture2D Textures[] : register(t0, space0);
SamplerState LinearSampler : register(s0, space0);

float4 main(PSInput input) : SV_TARGET
{
    float4 texColor = Textures[TextureIndex].Sample(LinearSampler, input.TexCoord);
    return texColor * input.Color;
})";

    std::vector<Byte> StringToByteArray( const char *str )
    {
        const size_t      len = strlen( str );
        std::vector<Byte> result( len );
        for ( size_t i = 0; i < len; i++ )
        {
            result[ i ] = static_cast<Byte>( str[ i ] );
        }
        return result;
    }
} // namespace

ImGuiBackend::ImGuiBackend( const ImGuiBackendDesc &desc ) : m_desc( desc ), m_logicalDevice( desc.LogicalDevice ), m_viewport( desc.Viewport ), m_numFrames( desc.NumFrames )
{
    if ( m_logicalDevice == nullptr )
    {
        spdlog::error( "ImGuiBackend: LogicalDevice cannot be null" );
        return;
    }

    if ( m_viewport.Width == 0 || m_viewport.Height == 0 )
    {
        spdlog::error( "ImGuiBackend: Viewport.Width and Viewport.Height must be greater than 0" );
        return;
    }

    m_textures.resize( desc.MaxTextures, nullptr );

    CommandQueueDesc commandQueueDesc{ };
    commandQueueDesc.QueueType = QueueType::Graphics;
    m_commandQueue             = std::unique_ptr<ICommandQueue>( m_logicalDevice->CreateCommandQueue( commandQueueDesc ) );

    CommandListPoolDesc poolDesc{ };
    poolDesc.CommandQueue    = m_commandQueue.get( );
    poolDesc.NumCommandLists = desc.NumFrames;
    m_commandListPool        = std::unique_ptr<ICommandListPool>( m_logicalDevice->CreateCommandListPool( poolDesc ) );

    CreateShaderProgram( );
    CreatePipeline( );
    CreateNullTexture( );
    CreateBuffers( );
    CreateFontTexture( );
    SetViewport( m_viewport );

    const auto commandLists = m_commandListPool->GetCommandLists( );
    m_frameData.resize( desc.NumFrames );
    for ( uint32_t i = 0; i < desc.NumFrames && i < commandLists.NumElements; ++i )
    {
        m_frameData[ i ].CommandList = commandLists.Elements[ i ];
        m_frameData[ i ].FrameFence  = std::unique_ptr<IFence>( m_logicalDevice->CreateFence( ) );
    }

    SamplerDesc samplerDesc{ };
    samplerDesc.AddressModeU = SamplerAddressMode::ClampToEdge;
    samplerDesc.AddressModeV = SamplerAddressMode::ClampToEdge;
    samplerDesc.AddressModeW = SamplerAddressMode::ClampToEdge;
    m_linearSampler          = std::unique_ptr<ISampler>( m_logicalDevice->CreateSampler( samplerDesc ) );

    UpdateTextureBindings( );
}

ImGuiBackend::~ImGuiBackend( )
{
    if ( m_vertexBufferData )
    {
        m_vertexBuffer->UnmapMemory( );
    }
    if ( m_indexBufferData )
    {
        m_indexBuffer->UnmapMemory( );
    }
    if ( m_uniformBufferData )
    {
        m_uniformBuffer->UnmapMemory( );
    }
    if ( m_pixelConstantsData )
    {
        m_pixelConstantsBuffer->UnmapMemory( );
    }
}

void ImGuiBackend::CreateShaderProgram( )
{
    auto vertexShader = StringToByteArray( ImGuiVertexShaderSource );
    auto pixelShader  = StringToByteArray( ImGuiPixelShaderSource );

    std::array<ShaderStageDesc, 2> shaderStages( { } );
    ShaderStageDesc               &vsDesc = shaderStages[ 0 ];
    vsDesc.Stage                          = ShaderStage::Vertex;
    vsDesc.EntryPoint                     = InteropString( "main" );
    vsDesc.Data.Elements                  = vertexShader.data( );
    vsDesc.Data.NumElements               = vertexShader.size( );

    ShaderStageDesc &psDesc = shaderStages[ 1 ];
    psDesc.Stage            = ShaderStage::Pixel;
    psDesc.EntryPoint       = InteropString( "main" );
    psDesc.Data.Elements    = pixelShader.data( );
    psDesc.Data.NumElements = pixelShader.size( );

    std::array<BindlessSlot, 1> bindlessSlots( { } );
    bindlessSlots[ 0 ].RegisterSpace = 0;
    bindlessSlots[ 0 ].Binding       = 0;
    bindlessSlots[ 0 ].MaxArraySize  = m_desc.MaxTextures;
    bindlessSlots[ 0 ].Type          = ResourceBindingType::ShaderResource;

    psDesc.Bindless.BindlessArrays.Elements    = bindlessSlots.data( );
    psDesc.Bindless.BindlessArrays.NumElements = bindlessSlots.size( );

    ShaderProgramDesc programDesc{ };
    programDesc.ShaderStages.Elements    = shaderStages.data( );
    programDesc.ShaderStages.NumElements = shaderStages.size( );
    m_shaderProgram                      = std::make_unique<ShaderProgram>( programDesc );
}

void ImGuiBackend::CreatePipeline( )
{
    const ShaderReflectDesc reflectDesc = m_shaderProgram->Reflect( );
    m_rootSignature                     = std::unique_ptr<IRootSignature>( m_logicalDevice->CreateRootSignature( reflectDesc.RootSignature ) );
    m_inputLayout                       = std::unique_ptr<IInputLayout>( m_logicalDevice->CreateInputLayout( reflectDesc.InputLayout ) );

    PipelineDesc pipelineDesc{ };
    pipelineDesc.RootSignature = m_rootSignature.get( );
    pipelineDesc.InputLayout   = m_inputLayout.get( );
    pipelineDesc.ShaderProgram = m_shaderProgram.get( );
    pipelineDesc.BindPoint     = BindPoint::Graphics;

    pipelineDesc.Graphics.PrimitiveTopology = PrimitiveTopology::Triangle;
    pipelineDesc.Graphics.CullMode          = CullMode::None;
    pipelineDesc.Graphics.FillMode          = FillMode::Solid;

    pipelineDesc.Graphics.DepthTest.Enable    = false;
    pipelineDesc.Graphics.DepthTest.CompareOp = CompareOp::Always;
    pipelineDesc.Graphics.DepthTest.Write     = false;

    RenderTargetDesc renderTarget;
    renderTarget.Format              = m_desc.RenderTargetFormat;
    renderTarget.Blend.Enable        = true;
    renderTarget.Blend.SrcBlend      = Blend::SrcAlpha;
    renderTarget.Blend.DstBlend      = Blend::InvSrcAlpha;
    renderTarget.Blend.BlendOp       = BlendOp::Add;
    renderTarget.Blend.SrcBlendAlpha = Blend::One;
    renderTarget.Blend.DstBlendAlpha = Blend::InvSrcAlpha;
    renderTarget.Blend.BlendOpAlpha  = BlendOp::Add;

    pipelineDesc.Graphics.RenderTargets.Elements    = &renderTarget;
    pipelineDesc.Graphics.RenderTargets.NumElements = 1;

    m_pipeline = std::unique_ptr<IPipeline>( m_logicalDevice->CreatePipeline( pipelineDesc ) );
}

void ImGuiBackend::CreateBuffers( )
{
    BufferDesc vertexBufferDesc{ };
    vertexBufferDesc.NumBytes   = m_desc.MaxVertices * sizeof( ImDrawVert );
    vertexBufferDesc.Descriptor = ResourceDescriptor::VertexBuffer;
    vertexBufferDesc.Usages     = ResourceUsage::VertexAndConstantBuffer;
    vertexBufferDesc.HeapType   = HeapType::CPU_GPU;
    vertexBufferDesc.DebugName  = InteropString( "ImGui Vertex Buffer" );
    m_vertexBuffer              = std::unique_ptr<IBufferResource>( m_logicalDevice->CreateBufferResource( vertexBufferDesc ) );
    m_vertexBufferData          = static_cast<uint8_t *>( m_vertexBuffer->MapMemory( ) );

    BufferDesc indexBufferDesc{ };
    indexBufferDesc.NumBytes   = m_desc.MaxIndices * sizeof( ImDrawIdx );
    indexBufferDesc.Descriptor = ResourceDescriptor::IndexBuffer;
    indexBufferDesc.Usages     = ResourceUsage::IndexBuffer;
    indexBufferDesc.HeapType   = HeapType::CPU_GPU;
    indexBufferDesc.DebugName  = InteropString( "ImGui Index Buffer" );
    m_indexBuffer              = std::unique_ptr<IBufferResource>( m_logicalDevice->CreateBufferResource( indexBufferDesc ) );
    m_indexBufferData          = static_cast<uint8_t *>( m_indexBuffer->MapMemory( ) );
    m_alignedUniformSize = sizeof( ImGuiUniforms ) + 255 & ~255;

    BufferDesc uniformBufferDesc{ };
    uniformBufferDesc.NumBytes   = m_alignedUniformSize * m_desc.NumFrames;
    uniformBufferDesc.Descriptor = ResourceDescriptor::UniformBuffer;
    uniformBufferDesc.Usages     = ResourceUsage::VertexAndConstantBuffer;
    uniformBufferDesc.HeapType   = HeapType::CPU_GPU;
    uniformBufferDesc.DebugName  = InteropString( "ImGui Uniform Buffer" );
    m_uniformBuffer              = std::unique_ptr<IBufferResource>( m_logicalDevice->CreateBufferResource( uniformBufferDesc ) );
    m_uniformBufferData          = static_cast<ImGuiUniforms *>( m_uniformBuffer->MapMemory( ) );
    m_alignedPixelConstantsSize  = sizeof( PixelConstants ) + 255 & ~255;

    BufferDesc pixelConstantsBufferDesc{ };
    pixelConstantsBufferDesc.NumBytes   = m_alignedPixelConstantsSize * m_desc.MaxTextures;
    pixelConstantsBufferDesc.Descriptor = ResourceDescriptor::UniformBuffer;
    pixelConstantsBufferDesc.Usages     = ResourceUsage::VertexAndConstantBuffer;
    pixelConstantsBufferDesc.HeapType   = HeapType::CPU_GPU;
    pixelConstantsBufferDesc.DebugName  = InteropString( "ImGui Pixel Constants Buffer" );
    m_pixelConstantsBuffer              = std::unique_ptr<IBufferResource>( m_logicalDevice->CreateBufferResource( pixelConstantsBufferDesc ) );
    m_pixelConstantsData                = static_cast<PixelConstants *>( m_pixelConstantsBuffer->MapMemory( ) );

    for ( uint32_t i = 0; i < m_desc.MaxTextures; ++i )
    {
        const auto pixelConstantsData    = reinterpret_cast<PixelConstants *>( reinterpret_cast<uint8_t *>( m_pixelConstantsData ) + i * m_alignedPixelConstantsSize );
        pixelConstantsData->TextureIndex = i;
    }

    m_frameData.resize( m_desc.NumFrames );
    for ( uint32_t i = 0; i < m_desc.NumFrames; ++i )
    {
        ResourceBindGroupDesc constantsBindGroupDesc{ };
        constantsBindGroupDesc.RootSignature = m_rootSignature.get( );
        constantsBindGroupDesc.RegisterSpace = 1;
        m_frameData[ i ].ConstantsBindGroup  = std::unique_ptr<IResourceBindGroup>( m_logicalDevice->CreateResourceBindGroup( constantsBindGroupDesc ) );

        BindBufferDesc bindUniformsDesc{ };
        bindUniformsDesc.Resource       = m_uniformBuffer.get( );
        bindUniformsDesc.ResourceOffset = i * m_alignedUniformSize;

        m_frameData[ i ].ConstantsBindGroup->BeginUpdate( )->Cbv( bindUniformsDesc )->EndUpdate( );

        ResourceBindGroupDesc textureBindGroupDesc{ };
        textureBindGroupDesc.RootSignature = m_rootSignature.get( );
        textureBindGroupDesc.RegisterSpace = 0;
        m_frameData[ i ].TextureBindGroup  = std::unique_ptr<IResourceBindGroup>( m_logicalDevice->CreateResourceBindGroup( textureBindGroupDesc ) );
    }

    m_pixelConstantsBindGroups.resize( m_desc.MaxTextures );
    for ( uint32_t i = 0; i < m_desc.MaxTextures; ++i )
    {
        ResourceBindGroupDesc pixelConstantsBindGroupDesc{ };
        pixelConstantsBindGroupDesc.RootSignature = m_rootSignature.get( );
        pixelConstantsBindGroupDesc.RegisterSpace = 2;
        m_pixelConstantsBindGroups[ i ]           = std::unique_ptr<IResourceBindGroup>( m_logicalDevice->CreateResourceBindGroup( pixelConstantsBindGroupDesc ) );

        BindBufferDesc bindPixelConstantsDesc{ };
        bindPixelConstantsDesc.Resource       = m_pixelConstantsBuffer.get( );
        bindPixelConstantsDesc.ResourceOffset = i * m_alignedPixelConstantsSize;

        m_pixelConstantsBindGroups[ i ]->BeginUpdate( );
        m_pixelConstantsBindGroups[ i ]->Cbv( bindPixelConstantsDesc );
        m_pixelConstantsBindGroups[ i ]->EndUpdate( );
    }
}

void ImGuiBackend::CreateNullTexture( )
{
    TextureDesc textureDesc{ };
    textureDesc.Width        = 1;
    textureDesc.Height       = 1;
    textureDesc.Format       = Format::R8G8B8A8Unorm;
    textureDesc.InitialUsage = ResourceUsage::Common;
    textureDesc.Usages       = ResourceUsage::ShaderResource;
    textureDesc.Descriptor   = ResourceDescriptor::Texture;
    textureDesc.HeapType     = HeapType::GPU;
    textureDesc.DebugName    = InteropString( "ImGui Null Texture" );

    m_nullTexture   = std::unique_ptr<ITextureResource>( m_logicalDevice->CreateTextureResource( textureDesc ) );
    m_textures[ 1 ] = m_nullTexture.get( );
}

void ImGuiBackend::CreateFontTexture( )
{
    ImGuiIO &io = ImGui::GetIO( );

    unsigned char *fontData;
    int            fontWidth, fontHeight;
    io.Fonts->GetTexDataAsRGBA32( &fontData, &fontWidth, &fontHeight );

    TextureDesc fontTextureDesc{ };
    fontTextureDesc.Width        = static_cast<uint32_t>( fontWidth );
    fontTextureDesc.Height       = static_cast<uint32_t>( fontHeight );
    fontTextureDesc.Format       = Format::R8G8B8A8Unorm;
    fontTextureDesc.InitialUsage = ResourceUsage::CopyDst;
    fontTextureDesc.Usages       = ResourceUsage::ShaderResource | ResourceUsage::CopyDst;
    fontTextureDesc.Descriptor   = ResourceDescriptor::Texture;
    fontTextureDesc.HeapType     = HeapType::GPU;
    fontTextureDesc.DebugName    = InteropString( "ImGui Font Texture" );
    m_fontTexture                = std::unique_ptr<ITextureResource>( m_logicalDevice->CreateTextureResource( fontTextureDesc ) );

    const size_t fontDataSize = fontWidth * fontHeight * 4;

    BufferDesc uploadBufferDesc{ };
    uploadBufferDesc.NumBytes   = fontDataSize;
    uploadBufferDesc.Descriptor = ResourceDescriptor::Buffer;
    uploadBufferDesc.Usages     = ResourceUsage::CopySrc;
    uploadBufferDesc.HeapType   = HeapType::CPU_GPU;
    uploadBufferDesc.DebugName  = InteropString( "ImGui Font Upload Buffer" );
    auto uploadBuffer           = std::unique_ptr<IBufferResource>( m_logicalDevice->CreateBufferResource( uploadBufferDesc ) );

    void *uploadData = uploadBuffer->MapMemory( );
    memcpy( uploadData, fontData, fontDataSize );
    uploadBuffer->UnmapMemory( );

    ResourceTracking resourceTracking;
    resourceTracking.TrackTexture( m_fontTexture.get( ), ResourceUsage::CopyDst );
    resourceTracking.TrackBuffer( uploadBuffer.get( ), ResourceUsage::CopySrc );

    auto commandList = m_commandListPool->GetCommandLists( ).Elements[ 0 ];
    commandList->Begin( );

    CopyBufferToTextureDesc copyDesc{ };
    copyDesc.DstTexture = m_fontTexture.get( );
    copyDesc.SrcBuffer  = uploadBuffer.get( );
    copyDesc.SrcOffset  = 0;
    copyDesc.DstX       = 0;
    copyDesc.DstY       = 0;
    copyDesc.DstZ       = 0;
    copyDesc.Format     = Format::R8G8B8A8Unorm;
    copyDesc.MipLevel   = 0;
    copyDesc.ArrayLayer = 0;
    copyDesc.RowPitch   = fontWidth * 4;
    copyDesc.NumRows    = fontHeight;

    commandList->CopyBufferToTexture( copyDesc );

    resourceTracking.TransitionTexture( commandList, m_fontTexture.get( ), ResourceUsage::ShaderResource );

    commandList->End( );

    auto uploadFence = std::unique_ptr<IFence>( m_logicalDevice->CreateFence( ) );

    ExecuteCommandListsDesc executeDesc{ };
    executeDesc.Signal                   = uploadFence.get( );
    executeDesc.CommandLists.Elements    = &commandList;
    executeDesc.CommandLists.NumElements = 1;
    executeDesc.WaitSemaphores           = { };
    executeDesc.SignalSemaphores         = { };

    m_commandQueue->ExecuteCommandLists( executeDesc );
    uploadFence->Wait( );

    m_textures[ 0 ] = m_fontTexture.get( );
    io.Fonts->SetTexID( 0 );
    m_texturesDirty = true;
}

void ImGuiBackend::SetViewport( const Viewport &viewport )
{
    m_viewport = viewport;

    constexpr float zn = 0.0f;
    constexpr float zf = 1.0f;

    m_projectionMatrix._11 = 2.0f / ( viewport.Width - viewport.X );
    m_projectionMatrix._22 = 2.0f / ( viewport.Y - viewport.Height );
    m_projectionMatrix._33 = 1.0f / ( zf - zn );
    m_projectionMatrix._41 = ( viewport.X + viewport.Width ) / ( viewport.X - viewport.Width );
    m_projectionMatrix._42 = ( viewport.Height + viewport.Y ) / ( viewport.Height - viewport.Y );
    m_projectionMatrix._43 = zn / ( zn - zf );
    m_projectionMatrix._44 = 1.0f;
}

const Viewport &ImGuiBackend::GetViewport( ) const
{
    return m_viewport;
}

void ImGuiBackend::RenderDrawData( ICommandList *commandList, ImDrawData *drawData, const uint32_t frameIndex )
{
    if ( !drawData || drawData->CmdListsCount == 0 )
    {
        return;
    }

    m_currentFrame = m_nextFrame;
    m_nextFrame    = ( m_nextFrame + 1 ) % m_numFrames;

    UpdateTextureBindings( );

    size_t totalVertexSize = 0;
    size_t totalIndexSize  = 0;
    for ( int n = 0; n < drawData->CmdListsCount; n++ )
    {
        const ImDrawList *cmdList = drawData->CmdLists[ n ];
        totalVertexSize += cmdList->VtxBuffer.Size * sizeof( ImDrawVert );
        totalIndexSize += cmdList->IdxBuffer.Size * sizeof( ImDrawIdx );
    }

    if ( totalVertexSize > m_desc.MaxVertices * sizeof( ImDrawVert ) || totalIndexSize > m_desc.MaxIndices * sizeof( ImDrawIdx ) )
    {
        spdlog::error( "ImGui draw data exceeds buffer capacity" );
        return;
    }

    uint32_t vertexOffset = 0;
    uint32_t indexOffset  = 0;

    for ( int n = 0; n < drawData->CmdListsCount; n++ )
    {
        const ImDrawList *cmdList = drawData->CmdLists[ n ];

        const size_t vertexDataSize = cmdList->VtxBuffer.Size * sizeof( ImDrawVert );
        const size_t indexDataSize  = cmdList->IdxBuffer.Size * sizeof( ImDrawIdx );

        memcpy( m_vertexBufferData + vertexOffset * sizeof( ImDrawVert ), cmdList->VtxBuffer.Data, vertexDataSize );
        memcpy( m_indexBufferData + indexOffset * sizeof( ImDrawIdx ), cmdList->IdxBuffer.Data, indexDataSize );

        vertexOffset += cmdList->VtxBuffer.Size;
        indexOffset += cmdList->IdxBuffer.Size;
    }

    SetupRenderState( commandList, drawData, frameIndex );

    vertexOffset = 0;
    indexOffset  = 0;
    for ( int n = 0; n < drawData->CmdListsCount; n++ )
    {
        const ImDrawList *cmdList = drawData->CmdLists[ n ];
        RenderImDrawList( commandList, const_cast<ImDrawList *>( cmdList ), vertexOffset, indexOffset );
        vertexOffset += cmdList->VtxBuffer.Size;
        indexOffset += cmdList->IdxBuffer.Size;
    }
}

void ImGuiBackend::RecreateFonts( )
{
    CreateFontTexture( );
}

void ImGuiBackend::SetupRenderState( ICommandList *commandList, ImDrawData *drawData, const uint32_t frameIndex ) const
{
    commandList->BindPipeline( m_pipeline.get( ) );

    commandList->BindVertexBuffer( m_vertexBuffer.get( ) );
    commandList->BindIndexBuffer( m_indexBuffer.get( ), sizeof( ImDrawIdx ) == 2 ? IndexType::Uint16 : IndexType::Uint32 );

    const auto uniformData  = reinterpret_cast<ImGuiUniforms *>( reinterpret_cast<uint8_t *>( m_uniformBufferData ) + frameIndex * m_alignedUniformSize );
    uniformData->Projection = m_projectionMatrix;
    uniformData->ScreenSize = { static_cast<float>( m_viewport.Width ), static_cast<float>( m_viewport.Height ) };

    commandList->BindResourceGroup( m_frameData[ frameIndex ].ConstantsBindGroup.get( ) );
    commandList->BindResourceGroup( m_frameData[ frameIndex ].TextureBindGroup.get( ) );
}

void ImGuiBackend::RenderImDrawList( ICommandList *commandList, ImDrawList *cmdList, const uint32_t vertexOffset, const uint32_t indexOffset ) const
{
    for ( int cmdIdx = 0; cmdIdx < cmdList->CmdBuffer.Size; cmdIdx++ )
    {
        const ImDrawCmd *pcmd = &cmdList->CmdBuffer[ cmdIdx ];
        if ( pcmd->UserCallback != nullptr )
        {
            if ( pcmd->UserCallback == ImDrawCallback_ResetRenderState )
            {
                SetupRenderState( commandList, ImGui::GetDrawData( ), m_currentFrame );
            }
            else
            {
                pcmd->UserCallback( cmdList, pcmd );
            }
        }
        else
        {
            const ImVec2 clipMin( pcmd->ClipRect.x, pcmd->ClipRect.y );
            const ImVec2 clipMax( pcmd->ClipRect.z, pcmd->ClipRect.w );
            if ( clipMax.x <= clipMin.x || clipMax.y <= clipMin.y )
            {
                continue;
            }

            commandList->BindScissorRect( static_cast<int32_t>( clipMin.x ), static_cast<int32_t>( clipMin.y ), static_cast<uint32_t>( clipMax.x - clipMin.x ),
                                          static_cast<uint32_t>( clipMax.y - clipMin.y ) );

            const auto textureIndex = static_cast<uint32_t>( pcmd->TextureId );
            if ( textureIndex < m_textures.size( ) && m_textures[ textureIndex ] != nullptr && textureIndex < m_pixelConstantsBindGroups.size( ) )
            {
                commandList->BindResourceGroup( m_pixelConstantsBindGroups[ textureIndex ].get( ) );
                commandList->DrawIndexed( pcmd->ElemCount, 1, pcmd->IdxOffset + indexOffset, static_cast<int32_t>( pcmd->VtxOffset + vertexOffset ), 0 );
            }
        }
    }
}

ImTextureID ImGuiBackend::AddTexture( ITextureResource *texture )
{
    if ( texture == nullptr )
    {
        return 0;
    }

    for ( size_t i = 2; i < m_textures.size( ); ++i )
    {
        if ( m_textures[ i ] == nullptr )
        {
            m_textures[ i ]               = texture;
            const auto textureId          = i;
            m_textureToIndex[ textureId ] = static_cast<uint32_t>( i );
            m_texturesDirty               = true;
            return textureId;
        }
    }

    spdlog::error( "ImGui texture capacity exceeded" );
    return 0;
}

void ImGuiBackend::RemoveTexture( const ImTextureID textureId )
{
    const auto index = static_cast<uint32_t>( textureId );
    if ( index < m_textures.size( ) && index >= 2 )
    {
        m_textures[ index ] = nullptr;
        m_textureToIndex.erase( textureId );
        m_texturesDirty = true;
    }
}

void ImGuiBackend::UpdateTextureBindings( )
{
    if ( !m_texturesDirty )
    {
        return;
    }

    for ( uint32_t frameIndex = 0; frameIndex < m_desc.NumFrames; ++frameIndex )
    {
        m_frameData[ frameIndex ].TextureBindGroup->BeginUpdate( );

        std::vector<ITextureResource *> textureVector( m_textures.size( ) );
        for ( size_t i = 0; i < m_textures.size( ); ++i )
        {
            textureVector[ i ] = m_textures[ i ] ? m_textures[ i ] : m_nullTexture.get( );
        }

        const TextureResourceArray textureArray{ textureVector.data( ), static_cast<uint32_t>( textureVector.size( ) ) };

        m_frameData[ frameIndex ].TextureBindGroup->SrvArray( 0, textureArray );
        m_frameData[ frameIndex ].TextureBindGroup->Sampler( 0, m_linearSampler.get( ) );
        m_frameData[ frameIndex ].TextureBindGroup->EndUpdate( );
    }

    m_texturesDirty = false;
}

void ImGuiBackend::ProcessEvent( const Event &event ) const
{
    ImGuiIO &io = ImGui::GetIO( );

    switch ( event.Type )
    {
    case EventType::MouseMotion:
        {
            io.AddMousePosEvent( event.MouseMotion.X, event.MouseMotion.Y );
            break;
        }
    case EventType::MouseButtonDown:
    case EventType::MouseButtonUp:
        {
            int mouseButton = 0;
            switch ( event.MouseButton.Button )
            {
            case MouseButton::Left:
                mouseButton = 0;
                break;
            case MouseButton::Right:
                mouseButton = 1;
                break;
            case MouseButton::Middle:
                mouseButton = 2;
                break;
            default:
                break;
            }
            io.AddMouseButtonEvent( mouseButton, event.Type == EventType::MouseButtonDown );
            break;
        }
    case EventType::MouseWheel:
        {
            io.AddMouseWheelEvent( event.MouseWheel.X, event.MouseWheel.Y );
            break;
        }
    case EventType::KeyDown:
    case EventType::KeyUp:
        {
            const ImGuiKey key = KeyCodeToImGuiKey( event.Key.KeyCode );
            io.AddKeyEvent( key, event.Type == EventType::KeyDown );
            break;
        }
    case EventType::TextInput:
        {
            io.AddInputCharactersUTF8( event.Text.Text );
            break;
        }
    default:
        break;
    }
}

ImGuiKey ImGuiBackend::KeyCodeToImGuiKey( const KeyCode keycode )
{
    switch ( keycode )
    {
    case KeyCode::Tab:
        return ImGuiKey_Tab;
    case KeyCode::Left:
        return ImGuiKey_LeftArrow;
    case KeyCode::Right:
        return ImGuiKey_RightArrow;
    case KeyCode::Up:
        return ImGuiKey_UpArrow;
    case KeyCode::Down:
        return ImGuiKey_DownArrow;
    case KeyCode::PageUp:
        return ImGuiKey_PageUp;
    case KeyCode::PageDown:
        return ImGuiKey_PageDown;
    case KeyCode::Home:
        return ImGuiKey_Home;
    case KeyCode::End:
        return ImGuiKey_End;
    case KeyCode::Insert:
        return ImGuiKey_Insert;
    case KeyCode::Delete:
        return ImGuiKey_Delete;
    case KeyCode::Backspace:
        return ImGuiKey_Backspace;
    case KeyCode::Space:
        return ImGuiKey_Space;
    case KeyCode::Return:
        return ImGuiKey_Enter;
    case KeyCode::Escape:
        return ImGuiKey_Escape;
    case KeyCode::LCtrl:
        return ImGuiKey_LeftCtrl;
    case KeyCode::LShift:
        return ImGuiKey_LeftShift;
    case KeyCode::LAlt:
        return ImGuiKey_LeftAlt;
    case KeyCode::LGui:
        return ImGuiKey_LeftSuper;
    case KeyCode::RCtrl:
        return ImGuiKey_RightCtrl;
    case KeyCode::RShift:
        return ImGuiKey_RightShift;
    case KeyCode::RAlt:
        return ImGuiKey_RightAlt;
    case KeyCode::RGui:
        return ImGuiKey_RightSuper;
    case KeyCode::A:
        return ImGuiKey_A;
    case KeyCode::C:
        return ImGuiKey_C;
    case KeyCode::V:
        return ImGuiKey_V;
    case KeyCode::X:
        return ImGuiKey_X;
    case KeyCode::Y:
        return ImGuiKey_Y;
    case KeyCode::Z:
        return ImGuiKey_Z;
    default:
        return ImGuiKey_None;
    }
}

namespace DenOfIz::ImGuiUtils
{
    ImGuiBackend *CreateImGuiBackend( const ImGuiBackendDesc &desc )
    {
        return new ImGuiBackend( desc );
    }

    void DestroyImGuiBackend( const ImGuiBackend *backend )
    {
        delete backend;
    }

    void InitializeImGui( ImGuiBackend *backend )
    {
    }

    void ShutdownImGui( )
    {
        ImGui::DestroyContext( );
    }
} // namespace DenOfIz::ImGuiUtils

ImGuiRenderer::ImGuiRenderer( const ImGuiBackendDesc &desc )
{
    IMGUI_CHECKVERSION( );
    ImGui::CreateContext( );
    ImGuiIO &io = ImGui::GetIO( );
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark( );

    io.BackendRendererName = "imgui_impl_denofiz";
    io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;

    m_backend = std::make_unique<ImGuiBackend>( desc );
}

ImGuiRenderer::~ImGuiRenderer( )
{
    ImGuiUtils::ShutdownImGui( );
}

void ImGuiRenderer::ProcessEvent( const Event &event ) const
{
    m_backend->ProcessEvent( event );
}

void ImGuiRenderer::NewFrame( const uint32_t width, const uint32_t height, const float deltaTime ) const
{
    ImGuiIO &io    = ImGui::GetIO( );
    io.DisplaySize = ImVec2( static_cast<float>( width ), static_cast<float>( height ) );
    io.DeltaTime   = deltaTime;
    ImGui::NewFrame( );
}

void ImGuiRenderer::Render( ITextureResource *renderTarget, ICommandList *commandList, const uint32_t frameIndex ) const
{
    RenderingAttachmentDesc attachmentDesc{ };
    attachmentDesc.Resource = renderTarget;

    RenderingDesc renderingDesc{ };
    renderingDesc.RTAttachments.Elements    = &attachmentDesc;
    renderingDesc.RTAttachments.NumElements = 1;

    commandList->BeginRendering( renderingDesc );

    const Viewport viewport = m_backend->GetViewport( );
    commandList->BindViewport( viewport.X, viewport.Y, viewport.Width, viewport.Height );
    commandList->BindScissorRect( viewport.X, viewport.Y, viewport.Width, viewport.Height );

    m_backend->RenderDrawData( commandList, ImGui::GetDrawData( ), frameIndex );

    commandList->EndRendering( );
}

void ImGuiRenderer::SetViewport( const Viewport viewport ) const
{
    m_backend->SetViewport( viewport );
}

void ImGuiRenderer::RecreateFonts( ) const
{
    m_backend->RecreateFonts( );
}

ImTextureID ImGuiRenderer::AddTexture( ITextureResource *texture ) const
{
    return m_backend->AddTexture( texture );
}

void ImGuiRenderer::RemoveTexture( const ImTextureID textureId ) const
{
    m_backend->RemoveTexture( textureId );
}
