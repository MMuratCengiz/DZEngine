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

    m_frameBindings.resize( m_numFrames );
    CreateSamplersBinding( );
    CreateBuffersBinding( );
    CreateTexturesBinding( );
}

void GPUDrivenBinding::Update( const uint32_t frameIndex ) const
{
    UpdateTextures( frameIndex );
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

        m_samplerBindGroup->BeginUpdate( );
        m_samplerBindGroup->Cbv( 0, buffers.GlobalDataBuffer );
        m_samplerBindGroup->Srv( 0, buffers.ObjectBuffer );
        m_samplerBindGroup->Srv( 1, buffers.MaterialBuffer );
        m_samplerBindGroup->Srv( 2, buffers.MeshBuffer );
        m_samplerBindGroup->Srv( 3, buffers.InstanceBuffer );
        m_samplerBindGroup->EndUpdate( );
    }
}

void GPUDrivenBinding::CreateTexturesBinding( ) const
{
    ResourceBindGroupDesc bindGroupDesc{ };
    bindGroupDesc.RegisterSpace = TexturesSpace;
    bindGroupDesc.RootSignature = m_rootSig->GetRootSignature( );

    for ( int i = 0; i < m_numFrames; ++i )
    {
        m_frameBindings[ i ]->TexturesBinding = std::unique_ptr<IResourceBindGroup>( m_graphicsContext->LogicalDevice->CreateResourceBindGroup( bindGroupDesc ) );
        UpdateTextures( i );
    }
}

void GPUDrivenBinding::UpdateTextures( uint32_t frameIndex ) const
{
    std::vector<ITextureResource *> textures;

    // WIP
}
