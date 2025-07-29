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

#include "DZEngine/GraphicsContext.h"

using namespace DZEngine;

GraphicsContext::GraphicsContext( const GraphicsContextDesc graphicsContextDesc ) : m_windowHandle( graphicsContextDesc.WindowHandle )
{
    APIPreference apiPreferences{ };
    apiPreferences.Windows = APIPreferenceWindows::DirectX12;
    apiPreferences.Linux   = APIPreferenceLinux::Vulkan;
    apiPreferences.OSX     = APIPreferenceOSX::Metal;
    apiPreferences.Web     = APIPreferenceWeb::WebGPU;
    const GraphicsApi graphicsApi( apiPreferences );
    m_logicalDevice = std::unique_ptr<ILogicalDevice>( graphicsApi.CreateAndLoadOptimalLogicalDevice( ) );

    CommandQueueDesc copyQueue{ };
    copyQueue.QueueType = QueueType::Copy;
    m_copyQueue.reset( m_logicalDevice->CreateCommandQueue( copyQueue ) );

    CommandQueueDesc graphicsQueue{ };
    graphicsQueue.QueueType = QueueType::Graphics;
    m_graphicsQueue.reset( m_logicalDevice->CreateCommandQueue( graphicsQueue ) );

    CommandQueueDesc computeQueue{ };
    computeQueue.QueueType = QueueType::Compute;
    m_computeQueue.reset( m_logicalDevice->CreateCommandQueue( computeQueue ) );

    CreateSwapChain( );
}

bool GraphicsContext::IsDeviceLost( ) const
{
    return m_deviceLost;
}

void GraphicsContext::HandleEvent( const Event &event ) const
{
    if ( event.Type == EventType::WindowEvent && event.Window.Event == WindowEventType::Resized )
    {
        m_logicalDevice->WaitIdle( );
        m_graphicsQueue->WaitIdle( );
        WaitIdle( );
        m_swapChain->Resize( event.Window.Data1, event.Window.Data2 );
        for ( uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++ )
        {
            m_resourceTracking->TrackTexture( m_swapChain->GetRenderTarget( i ), ResourceUsage::Common );
        }
    }
}

ICommandQueue *GraphicsContext::CopyQueue( ) const
{
    return m_copyQueue.get( );
}

ICommandQueue *GraphicsContext::GraphicsQueue( ) const
{
    return m_graphicsQueue.get( );
}

ICommandQueue *GraphicsContext::ComputeQueue( ) const
{
    return m_computeQueue.get( );
}

ILogicalDevice *GraphicsContext::GetLogicalDevice( ) const
{
    return m_logicalDevice.get( );
}

ISwapChain *GraphicsContext::GetSwapChain( ) const
{
    return m_swapChain.get( );
}

ResourceTracking *GraphicsContext::GetResourceTracking( ) const
{
    return m_resourceTracking.get( );
}

void GraphicsContext::WaitIdle( ) const
{
    m_logicalDevice->WaitIdle( );
}

uint32_t GraphicsContext::AcquireNextImage( ) const
{
    return m_swapChain->AcquireNextImage( );
}

ITextureResource *GraphicsContext::GetSwapChainRenderTarget( const uint32_t index ) const
{
    return m_swapChain->GetRenderTarget( index );
}

void GraphicsContext::Present( const uint32_t imageIndex )
{
    switch ( m_swapChain->Present( imageIndex ) )
    {
    case PresentResult::Success:
    case PresentResult::Suboptimal:
        break;
    case PresentResult::Timeout:
    case PresentResult::DeviceLost:
        m_logicalDevice->WaitIdle( );
        m_swapChain.reset( );
        CreateSwapChain( );
        break;
    }
}

void GraphicsContext::CreateSwapChain( )
{
    SwapChainDesc swapChainDesc{ };
    swapChainDesc.WindowHandle      = m_windowHandle;
    swapChainDesc.CommandQueue      = m_graphicsQueue.get( );
    swapChainDesc.Width             = m_windowHandle->GetSurface( ).Width;
    swapChainDesc.Height            = m_windowHandle->GetSurface( ).Height;
    swapChainDesc.NumBuffers        = MAX_FRAMES_IN_FLIGHT;
    swapChainDesc.BackBufferFormat  = Format::B8G8R8A8Unorm;
    swapChainDesc.DepthBufferFormat = Format::D32Float;
    swapChainDesc.ImageUsages       = ResourceUsage::RenderTarget | ResourceUsage::Present;
    swapChainDesc.AllowTearing      = true;
    swapChainDesc.EnableHDR         = false;
    swapChainDesc.ColorSpace        = ColorSpace::sRGB;
    swapChainDesc.SampleCount       = MSAASampleCount::_1;
    m_swapChain.reset( m_logicalDevice->CreateSwapChain( swapChainDesc ) );

    for ( uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++ )
    {
        m_resourceTracking->TrackTexture( m_swapChain->GetRenderTarget( i ), ResourceUsage::Common );
    }
}
