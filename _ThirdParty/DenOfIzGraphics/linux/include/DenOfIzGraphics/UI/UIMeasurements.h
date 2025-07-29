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

// ReSharper disable CppNonExplicitConvertingConstructor
#pragma once

namespace DenOfIz
{
    // Type-safe UI measurement units following industry standards
    // Points: Device-independent logical pixels
    // Pixels: Device-dependent physical pixels, dpiScaled
    struct UIPoints
    {
        float value;

        constexpr UIPoints( const int v ) : value( static_cast<float>( v ) ) // NOLINT(*-explicit-constructor)
        {
        }
        constexpr UIPoints( const float v ) : value( v ) // NOLINT(*-explicit-constructor)
        {
        }
        constexpr UIPoints( ) : value( 0.0f )
        {
        }
        constexpr UIPoints( const size_t v ) : value( static_cast<float>( v ) ) // NOLINT(*-explicit-constructor)
        {
        }
        explicit UIPoints( const double v ) : value( static_cast<float>( v ) )
        {
        }
        explicit UIPoints( const uint16_t v ) : value( static_cast<float>( v ) )
        {
        }
        explicit UIPoints( const uint32_t v ) : value( static_cast<float>( v ) )
        {
        }
        explicit operator float( ) const
        {
            return value;
        }

        UIPoints operator+( const UIPoints &other ) const
        {
            return { value + other.value };
        }
        UIPoints operator-( const UIPoints &other ) const
        {
            return { value - other.value };
        }
        UIPoints operator*( const float scale ) const
        {
            return { value * scale };
        }
        UIPoints operator/( const float scale ) const
        {
            return { value / scale };
        }
        UIPoints &operator+=( const UIPoints &other )
        {
            value += other.value;
            return *this;
        }
        UIPoints &operator-=( const UIPoints &other )
        {
            value -= other.value;
            return *this;
        }
        UIPoints &operator*=( const float scale )
        {
            value *= scale;
            return *this;
        }
        UIPoints &operator/=( const float scale )
        {
            value /= scale;
            return *this;
        }

        bool operator==( const UIPoints &other ) const
        {
            return value == other.value;
        }
        bool operator!=( const UIPoints &other ) const
        {
            return value != other.value;
        }
        bool operator<( const UIPoints &other ) const
        {
            return value < other.value;
        }
        bool operator>( const UIPoints &other ) const
        {
            return value > other.value;
        }
    };

    struct UIPixels
    {
        float value;
        explicit UIPixels( const float v ) : value( v )
        {
        }
        UIPixels( ) : value( 0.0f )
        {
        }

        explicit operator float( ) const
        {
            return value;
        }
    };

} // namespace DenOfIz
