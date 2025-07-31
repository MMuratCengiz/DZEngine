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

#pragma once

#include "GraphicsContext.h"

namespace DZEngine
{
    struct RenderLoopDesc
    {
        GraphicsWindowHandle *WindowHandle;
    };

    struct FrameState
    {
        bool              IsDeviceBusy; // Do not render if the device is busy, ie. Resizing
        uint32_t          FrameIndex;
        ITextureResource *RenderTarget;
        IFence           *NotifyFence;
    };

    class RenderLoop
    {
        constexpr static uint32_t MAX_FRAMES_IN_FLIGHT = 3; // Todo make this configurable

        GraphicsWindowHandle             *m_windowHandle;
        std::unique_ptr<ILogicalDevice>   m_logicalDevice;
        std::unique_ptr<ISwapChain>       m_swapChain;
        std::unique_ptr<ResourceTracking> m_resourceTracking;
        std::unique_ptr<ICommandQueue>    m_copyQueue;
        std::unique_ptr<ICommandQueue>    m_graphicsQueue;
        std::unique_ptr<ICommandQueue>    m_computeQueue;

        std::unique_ptr<GraphicsContext> m_graphicsContext;

        std::vector<std::unique_ptr<IFence>> m_frameFences;
        uint32_t                             m_currentFrame = 0;
        uint32_t                             m_nextFrame    = 0;

        bool m_deviceBusy = false;

    public:
        explicit RenderLoop( RenderLoopDesc renderLoopDesc );
        ~RenderLoop( );
        [[nodiscard]] GraphicsContext *GetGraphicsContext( ) const;
        [[nodiscard]] FrameState       NextFrame( );
        void                           Present( ) const;
        void                           HandleEvent( const Event &event );

    private:
        void Present( uint32_t imageIndex );
        void CreateSwapChain( );
    };
} // namespace DZEngine
