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

#include <cstdint>
#include "Common_Arrays.h"

/*
 * NOTE:
 *  This class is mostly used to specify types, it is not intended to provide full math functions, please convert them to relevant types of the Math library you use.
 */

namespace DenOfIz
{
    struct DZ_API Float2
    {
        float X;
        float Y;
    };

    struct DZ_API Float2Array
    {
        Float2 *Elements;
        size_t  NumElements;
    };

    struct DZ_API Float3
    {
        float X;
        float Y;
        float Z;
    };

    struct DZ_API Float3Array
    {
        Float3 *Elements;
        size_t  NumElements;
    };

    struct DZ_API Float4
    {
        float X;
        float Y;
        float Z;
        float W;
    };

    typedef Float4 Quaternion;

    struct DZ_API Float4Array
    {
        Float4 *Elements;
        size_t  NumElements;
    };

    struct DZ_API Short2
    {
        int16_t X;
        int16_t Y;
    };

    struct DZ_API Short3
    {
        int16_t X;
        int16_t Y;
        int16_t Z;
    };

    struct DZ_API Short4
    {
        int16_t X;
        int16_t Y;
        int16_t Z;
        int16_t W;
    };

    struct DZ_API Int2
    {
        int X;
        int Y;
    };

    struct DZ_API Int3
    {
        int X;
        int Y;
        int Z;
    };

    struct DZ_API Int4
    {
        int X;
        int Y;
        int Z;
        int W;
    };

    struct DZ_API UShort2
    {
        uint16_t X;
        uint16_t Y;
    };

    struct DZ_API UShort3
    {
        uint16_t X;
        uint16_t Y;
        uint16_t Z;
    };

    struct DZ_API UShort4
    {
        uint16_t X;
        uint16_t Y;
        uint16_t Z;
        uint16_t W;
    };

    struct DZ_API UInt2
    {
        uint32_t X;
        uint32_t Y;
    };

    struct DZ_API UInt3
    {
        uint32_t X;
        uint32_t Y;
        uint32_t Z;
    };

    struct DZ_API UInt4
    {
        uint32_t X;
        uint32_t Y;
        uint32_t Z;
        uint32_t W;
    };

    struct Float4x4
    {
        float _11 = 1, _12 = 0, _13 = 0, _14 = 0;
        float _21 = 0, _22 = 1, _23 = 0, _24 = 0;
        float _31 = 0, _32 = 0, _33 = 1, _34 = 0;
        float _41 = 0, _42 = 0, _43 = 0, _44 = 1;

        [[nodiscard]] float GetElement( const uint32_t major, const uint32_t minor ) const
        {
            if ( major >= 4 || minor >= 4 )
            {
                return 0.0f;
            }

            const float *base = &_11;
            return base[ major * 4 + minor ];
        }

        void SetElement( const uint32_t major, const uint32_t minor, const float value )
        {
            if ( major >= 4 || minor >= 4 )
            {
                return;
            }

            float *base               = &_11;
            base[ major * 4 + minor ] = value;
        }
    };

    struct DZ_API Float4x4Array
    {
        Float4x4 *Elements;
        size_t    NumElements;
    };
} // namespace DenOfIz
