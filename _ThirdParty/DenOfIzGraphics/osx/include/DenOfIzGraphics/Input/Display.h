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
    struct DZ_API DisplaySize
    {
        int Width;
        int Height;
    };

    struct DZ_API Display
    {
        uint32_t    ID;
        DisplaySize Size;      // Physical pixel size (DPI-scaled)
        float       DpiScale;  // DPI scale factor (1.0 = no scaling, 2.0 = 2x scaling)
        bool        IsPrimary; // Whether this is the primary display

        struct Array
        {
            Display *Elements    = nullptr;
            size_t   NumElements = 0;
        };

        static Array   GetDisplays( );
        static Display GetPrimaryDisplay( );
        static void    RefreshDisplays( );
    };
} // namespace DenOfIz
