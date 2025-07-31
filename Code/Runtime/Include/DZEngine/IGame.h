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

#include "AppContext.h"
#include "DenOfIzGraphics/DenOfIzGraphics.h"

using namespace DenOfIz;

namespace DZEngine
{
    struct RenderDesc
    {
        uint32_t          FrameIndex      = 0;
        IFence           *NotifyFence     = nullptr; // Can be null
        ISemaphore       *NotifySemaphore = nullptr; // Can be null
        ITextureResource *RenderTarget    = nullptr;
        Viewport          Viewport{ };
    };

    class IGame
    {
    public:
        virtual ~IGame( )                              = default;
        virtual void Init( AppContext *appContext )    = 0;
        virtual void HandleEvent( const Event &event ) = 0;
        virtual void Update( )                         = 0;
        // Return false if didn't render to indicate semaphore will not be singled
        virtual bool Render( const RenderDesc &renderDesc ) = 0;
    };
} // namespace DZEngine
