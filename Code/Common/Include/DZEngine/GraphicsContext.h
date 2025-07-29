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

#include "DenOfIzGraphics/DenOfIzGraphics.h"

namespace DZEngine
{
    struct GraphicsContextDesc
    {
        GraphicsWindowHandle *WindowHandle;
    };

    class GraphicsContext
    {
        constexpr static uint32_t MAX_FRAMES_IN_FLIGHT = 3;

        GraphicsWindowHandle             *m_windowHandle;
        std::unique_ptr<ILogicalDevice>   m_logicalDevice;
        std::unique_ptr<ISwapChain>       m_swapChain;
        std::unique_ptr<ResourceTracking> m_resourceTracking;
        std::unique_ptr<ICommandQueue>    m_copyQueue;
        std::unique_ptr<ICommandQueue>    m_graphicsQueue;
        std::unique_ptr<ICommandQueue>    m_computeQueue;

        bool                       m_deviceLost = false;
        std::unique_ptr<FrameSync> m_frameSync;

    public:
        explicit GraphicsContext( GraphicsContextDesc graphicsContextDesc );
        [[nodiscard]] bool IsDeviceLost( ) const; // must halt rendering operations
        void               HandleEvent( const Event &event ) const;

        [[nodiscard]] ICommandQueue    *CopyQueue( ) const;
        [[nodiscard]] ICommandQueue    *GraphicsQueue( ) const;
        [[nodiscard]] ICommandQueue    *ComputeQueue( ) const;
        [[nodiscard]] ILogicalDevice   *GetLogicalDevice( ) const;
        [[nodiscard]] ISwapChain       *GetSwapChain( ) const;
        [[nodiscard]] ResourceTracking *GetResourceTracking( ) const;

        void WaitIdle( ) const;

        uint32_t          NextFrame( ) const;
        uint32_t          AcquireNextImage( ) const;
        ITextureResource *GetSwapChainRenderTarget( uint32_t index ) const;
        void              Present( uint32_t imageIndex );

        ~GraphicsContext( ) = default;

    private:
        void CreateSwapChain( );
    };
} // namespace DZEngine
