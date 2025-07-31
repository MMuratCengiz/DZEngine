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

#include <imgui.h>

namespace DZEngine
{
    enum class Theme
    {
        Dark,
        Light,
        Default
    };

    class EditorStyle
    {
    public:
        static void ApplyTheme( Theme theme );
        static void ApplyDefaultStyle( );
        static void ApplyDarkStyle( );
        static void ApplyLightStyle( );

        static void PushItemSpacing( float x, float y );
        static void PopItemSpacing( );

        static void PushFramePadding( float x, float y );
        static void PopFramePadding( );

        static void BeginToolbar( );
        static void EndToolbar( );

        static void BeginDockSpace( );
        static void EndDockSpace( );

    private:
        static void SetupColors( const ImVec4 &background, const ImVec4 &text, const ImVec4 &border, const ImVec4 &button, const ImVec4 &buttonHovered,
                                 const ImVec4 &buttonActive );
        static void SetupSizes( );
    };

    namespace Colors
    {
        constexpr inline auto DARK_BG     = ImVec4( 0.22f, 0.22f, 0.22f, 1.0f );
        constexpr inline auto DARK_PANEL  = ImVec4( 0.16f, 0.16f, 0.16f, 1.0f );
        constexpr inline auto DARK_HEADER = ImVec4( 0.26f, 0.26f, 0.26f, 1.0f );
        constexpr inline auto DARK_TEXT   = ImVec4( 0.9f, 0.9f, 0.9f, 1.0f );
        constexpr inline auto DARK_BORDER = ImVec4( 0.08f, 0.08f, 0.08f, 1.0f );
        constexpr inline auto BLUE        = ImVec4( 0.2f, 0.4f, 0.8f, 1.0f );
        constexpr inline auto BLUE_HOVER  = ImVec4( 0.3f, 0.5f, 0.9f, 1.0f );
        constexpr inline auto BLUE_ACTIVE = ImVec4( 0.1f, 0.3f, 0.7f, 1.0f );
    } // namespace Colors
} // namespace DZEngine
