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

#include "DZEngine/Rendering/GPUDriven/GPUDrivenBinding.h"

#include <spdlog/spdlog.h>

using namespace DZEngine;

GPUDrivenBinding::GPUDrivenBinding( const GPUDrivenBindingDesc &bindingDesc )
{
    if ( !bindingDesc.AppContext )
    {
        spdlog::error( "GPUDrivenBinding: AppContext is required" );
        return;
    }
    if ( !bindingDesc.RootSig )
    {
        spdlog::error( "GPUDrivenBinding: RootSig is required" );
        return;
    }
    if ( !bindingDesc.DataUpload )
    {
        spdlog::error( "GPUDrivenBinding: DataUpload is required" );
    }

    m_graphicsContext = bindingDesc.AppContext->GraphicsContext;
    m_numFrames       = bindingDesc.AppContext->NumFrames;
    m_rootSig         = bindingDesc.RootSig;
    m_dataUpload      = bindingDesc.DataUpload;
    m_assetBatcher    = bindingDesc.AppContext->AssetBatcher;
    m_world           = bindingDesc.AppContext->World;
    m_batchId         = bindingDesc.BatchId;

    m_frameBindings.resize( m_numFrames );
    for ( int i = 0; i < m_numFrames; ++i )
    {
        m_frameBindings[ i ] = std::make_unique<FrameBinding>( );
    }

    CreateSamplersBinding( );
    CreateBuffersBinding( );
    CreateTexturesBinding( );
}

void GPUDrivenBinding::Update( const uint32_t frameIndex ) const
{
    UpdateTextures( frameIndex );
}

IResourceBindGroup *GPUDrivenBinding::GetSamplerBinding( ) const
{
    return m_samplerBindGroup.get( );
}

IResourceBindGroup *GPUDrivenBinding::GetBuffersBinding( const uint32_t frameIndex ) const
{
    return m_frameBindings[ frameIndex ]->BuffersBinding.get( );
}

IResourceBindGroup *GPUDrivenBinding::GetTexturesBinding( const uint32_t frameIndex ) const
{
    return m_frameBindings[ frameIndex ]->TexturesBinding.get( );
}

void GPUDrivenBinding::CreateSamplersBinding( )
{
    SamplerDesc linearSamplerDesc{ };
    linearSamplerDesc.MinFilter     = Filter::Linear;
    linearSamplerDesc.MagFilter     = Filter::Linear;
    linearSamplerDesc.MipmapMode    = MipmapMode::Linear;
    linearSamplerDesc.AddressModeU  = SamplerAddressMode::Repeat;
    linearSamplerDesc.AddressModeV  = SamplerAddressMode::Repeat;
    linearSamplerDesc.AddressModeW  = SamplerAddressMode::Repeat;
    linearSamplerDesc.MaxAnisotropy = 16.0f;
    linearSamplerDesc.DebugName     = "LinearSampler";
    m_linearSampler                 = std::unique_ptr<ISampler>( m_graphicsContext->LogicalDevice->CreateSampler( linearSamplerDesc ) );

    SamplerDesc pointSamplerDesc{ };
    pointSamplerDesc.MinFilter    = Filter::Nearest;
    pointSamplerDesc.MagFilter    = Filter::Nearest;
    pointSamplerDesc.MipmapMode   = MipmapMode::Nearest;
    pointSamplerDesc.AddressModeU = SamplerAddressMode::Repeat;
    pointSamplerDesc.AddressModeV = SamplerAddressMode::Repeat;
    pointSamplerDesc.AddressModeW = SamplerAddressMode::Repeat;
    pointSamplerDesc.DebugName    = "PointSampler";
    m_pointSampler                = std::unique_ptr<ISampler>( m_graphicsContext->LogicalDevice->CreateSampler( pointSamplerDesc ) );

    SamplerDesc anisotropicSamplerDesc{ };
    anisotropicSamplerDesc.MinFilter     = Filter::Linear;
    anisotropicSamplerDesc.MagFilter     = Filter::Linear;
    anisotropicSamplerDesc.MipmapMode    = MipmapMode::Linear;
    anisotropicSamplerDesc.AddressModeU  = SamplerAddressMode::Repeat;
    anisotropicSamplerDesc.AddressModeV  = SamplerAddressMode::Repeat;
    anisotropicSamplerDesc.AddressModeW  = SamplerAddressMode::Repeat;
    anisotropicSamplerDesc.MaxAnisotropy = 16.0f;
    anisotropicSamplerDesc.DebugName     = "AnisotropicSampler";
    m_anisotropicSampler                 = std::unique_ptr<ISampler>( m_graphicsContext->LogicalDevice->CreateSampler( anisotropicSamplerDesc ) );

    ResourceBindGroupDesc bindGroupDesc{ };
    bindGroupDesc.RegisterSpace = SamplerSpace;
    bindGroupDesc.RootSignature = m_rootSig->GetRootSignature( );
    m_samplerBindGroup          = std::unique_ptr<IResourceBindGroup>( m_graphicsContext->LogicalDevice->CreateResourceBindGroup( bindGroupDesc ) );

    m_samplerBindGroup->BeginUpdate( );
    m_samplerBindGroup->Sampler( 0, m_linearSampler.get( ) );
    m_samplerBindGroup->Sampler( 1, m_pointSampler.get( ) );
    m_samplerBindGroup->Sampler( 2, m_anisotropicSampler.get( ) );
    m_samplerBindGroup->EndUpdate( );
}

void GPUDrivenBinding::CreateBuffersBinding( ) const
{
    ResourceBindGroupDesc bindGroupDesc{ };
    bindGroupDesc.RegisterSpace = BuffersSpace;
    bindGroupDesc.RootSignature = m_rootSig->GetRootSignature( );

    for ( int i = 0; i < m_numFrames; ++i )
    {
        m_frameBindings[ i ]->BuffersBinding = std::unique_ptr<IResourceBindGroup>( m_graphicsContext->LogicalDevice->CreateResourceBindGroup( bindGroupDesc ) );

        const GPUDrivenBuffers buffers = m_dataUpload->GetBuffers( i );

        m_frameBindings[ i ]->BuffersBinding->BeginUpdate( );
        m_frameBindings[ i ]->BuffersBinding->Cbv( 0, buffers.GlobalDataBuffer );
        m_frameBindings[ i ]->BuffersBinding->Srv( 0, buffers.ObjectBuffer );
        m_frameBindings[ i ]->BuffersBinding->Srv( 1, buffers.MaterialBuffer );
        m_frameBindings[ i ]->BuffersBinding->Srv( 2, buffers.MeshBuffer );
        m_frameBindings[ i ]->BuffersBinding->Srv( 3, buffers.InstanceBuffer );

        if ( const auto meshBatch = m_assetBatcher->Mesh( m_batchId ) )
        {
            const auto vb = meshBatch->GetVertexBuffer( );
            const auto ib = meshBatch->GetIndexBuffer( );
            m_frameBindings[ i ]->BuffersBinding->Srv( 4, vb.Buffer );
            m_frameBindings[ i ]->BuffersBinding->Srv( 5, ib.Buffer );
        }

        m_frameBindings[ i ]->BuffersBinding->Srv( 6, buffers.DrawArgsBuffer );
        m_frameBindings[ i ]->BuffersBinding->EndUpdate( );
    }
}

void GPUDrivenBinding::CreateTexturesBinding( )
{
    ResourceBindGroupDesc bindGroupDesc{ };
    bindGroupDesc.RegisterSpace = TexturesSpace;
    bindGroupDesc.RootSignature = m_rootSig->GetRootSignature( );

    for ( int i = 0; i < m_numFrames; ++i )
    {
        m_frameBindings[ i ]->TexturesBinding = std::unique_ptr<IResourceBindGroup>( m_graphicsContext->LogicalDevice->CreateResourceBindGroup( bindGroupDesc ) );
        UpdateTextures( i );
    }

    TextureDesc textureDesc{ };
    textureDesc.Width        = 1;
    textureDesc.Height       = 1;
    textureDesc.Format       = Format::R8G8B8A8Unorm;
    textureDesc.InitialUsage = ResourceUsage::Common;
    textureDesc.Usages       = ResourceUsage::ShaderResource;
    textureDesc.Descriptor   = ResourceDescriptor::Texture;
    textureDesc.HeapType     = HeapType::GPU;
    textureDesc.DebugName    = InteropString( "UI Null Texture" );

    m_nullTexture = std::unique_ptr<ITextureResource>( m_graphicsContext->LogicalDevice->CreateTextureResource( textureDesc ) );
}

void GPUDrivenBinding::UpdateTextures( const uint32_t frameIndex ) const
{
    std::vector<ITextureResource *> textures;

    const auto texHandles = m_assetBatcher->Material( m_batchId )->GetTextures( );
    textures.reserve( texHandles.size( ) );
    for ( const auto &batchTextures : texHandles )
    {
        textures[ batchTextures.Handle.Id ] = batchTextures.Texture;
        if ( batchTextures.Texture == nullptr )
        {
            textures[ batchTextures.Handle.Id ] = m_nullTexture.get( );
        }
    }

    TextureResourceArray textureArray{ };
    textureArray.Elements    = textures.data( );
    textureArray.NumElements = static_cast<uint32_t>( textures.size( ) );

    m_frameBindings[ frameIndex ]->TexturesBinding->BeginUpdate( );
    m_frameBindings[ frameIndex ]->TexturesBinding->SrvArray( 0, textureArray );
    m_frameBindings[ frameIndex ]->TexturesBinding->EndUpdate( );
}
