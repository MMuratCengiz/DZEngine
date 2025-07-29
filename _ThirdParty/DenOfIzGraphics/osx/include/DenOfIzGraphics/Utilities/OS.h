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

#include "DenOfIzGraphics/Utilities/Common.h"

namespace DenOfIz
{
    namespace OS
    {
        /**
         * Get the display scale factor for a given display.
         * On macOS, this returns the backing scale factor (e.g., 2.0 for Retina displays).
         * On Windows/Linux, this returns 1.0 for now.
         * 
         * @param display The display index
         * @return The scale factor (1.0 = no scaling, 2.0 = 2x scaling, etc.)
         */
        DZ_API float GetDisplayScale( int display );
    }
}