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

#include "DZEngine/Rendering/RenderLoop.h"

using namespace DZEngine;

RenderLoop::RenderLoop( const RenderLoopDesc renderLoopDesc ) : m_windowHandle( renderLoopDesc.WindowHandle )
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

    m_graphicsContext                    = std::make_unique<GraphicsContext>( );
    m_graphicsContext->NumFramesInFlight = MAX_FRAMES_IN_FLIGHT;
    m_graphicsContext->LogicalDevice     = m_logicalDevice.get( );
    m_graphicsContext->CopyQueue         = m_copyQueue.get( );
    m_graphicsContext->GraphicsQueue     = m_graphicsQueue.get( );
    m_graphicsContext->ComputeQueue      = m_computeQueue.get( );
    m_graphicsContext->ResourceTracking  = m_resourceTracking.get( );
    m_graphicsContext->SwapChain         = m_swapChain.get( );

    m_frameFences.resize( MAX_FRAMES_IN_FLIGHT );
    for ( int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++ )
    {
        m_frameFences[ i ] = std::unique_ptr<IFence>( m_logicalDevice->CreateFence( ) );
    }
}

RenderLoop::~RenderLoop( ) = default;

GraphicsContext *RenderLoop::GetGraphicsContext( ) const
{
    return m_graphicsContext.get( );
}

FrameState RenderLoop::NextFrame( )
{
    FrameState frameState{ };
    frameState.IsDeviceBusy = m_deviceBusy;

    if ( m_deviceBusy )
    {
        frameState.RenderTarget = nullptr;
        return frameState;
    }

    m_currentFrame = m_nextFrame;
    m_nextFrame    = ( m_nextFrame + 1 ) % MAX_FRAMES_IN_FLIGHT;

    m_frameFences[ m_currentFrame ]->Wait( );
    const uint32_t imageIndex = m_swapChain->AcquireNextImage( );

    frameState.FrameIndex   = m_currentFrame;
    frameState.NotifyFence  = m_frameFences[ m_currentFrame ].get( );
    frameState.RenderTarget = m_swapChain->GetRenderTarget( imageIndex );
    return frameState;
}

void RenderLoop::Present( ) const
{
    m_swapChain->Present( m_currentFrame );
}

void RenderLoop::HandleEvent( const Event &event )
{
    if ( event.Type == EventType::WindowEvent && event.Window.Event == WindowEventType::Resized )
    {
        m_logicalDevice->WaitIdle( );
        m_graphicsQueue->WaitIdle( );
        m_deviceBusy = true;
        m_swapChain->Resize( event.Window.Data1, event.Window.Data2 );
        for ( uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++ )
        {
            m_resourceTracking->TrackTexture( m_swapChain->GetRenderTarget( i ), ResourceUsage::Common );
        }
        m_deviceBusy = false;
    }
}

void RenderLoop::Present( const uint32_t imageIndex )
{
    switch ( m_swapChain->Present( imageIndex ) )
    {
    case PresentResult::Success:
    case PresentResult::Suboptimal:
        break;
    case PresentResult::Timeout:
    case PresentResult::DeviceLost:
        m_deviceBusy = true;
        m_logicalDevice->WaitIdle( );
        m_swapChain.reset( );
        CreateSwapChain( );
        m_deviceBusy = false;
        break;
    }
}

void RenderLoop::CreateSwapChain( )
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
