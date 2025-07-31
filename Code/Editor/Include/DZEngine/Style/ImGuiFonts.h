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
#include <string>
#include <unordered_map>

namespace DZEngine
{
    enum class FontSize
    {
        VerySmall,
        Small,
        Medium,
        Large,
        VeryLarge
    };

    enum class FontType
    {
        Regular,
        Bold,
        Italic,
        Icon
    };

    struct FontConfig
    {
        float VerySmallSize     = 16.0f;
        float SmallSize         = 20.0f;
        float MediumSize        = 24.0f;
        float LargeSize         = 28.0f;
        float VeryLargeSize     = 32.0f;
        float IconVerySmallSize = 16.0f;
        float IconSmallSize     = 20.0f;
        float IconMediumSize    = 32.0f;
        float IconLargeSize     = 48.0f;
        float IconVeryLargeSize = 64.0f;
        bool  MergeIcons        = true;
    };

    class ImGuiFonts
    {
    public:
        static ImGuiFonts &Instance( );

        void Initialize( const FontConfig &config = FontConfig{ } );
        void LoadFonts( );

        ImFont *GetFont( FontType type, FontSize size );
        ImFont *GetIconFont( FontSize size );
        ImFont *GetDefaultFont( ) const;

        void PushFont( FontType type, FontSize size );
        void PopFont( ) const;

        void PushIconFont( FontSize size );
        void PopIconFont( ) const;

    private:
        ImGuiFonts( )                               = default;
        ~ImGuiFonts( )                              = default;
        ImGuiFonts( const ImGuiFonts & )            = delete;
        ImGuiFonts &operator=( const ImGuiFonts & ) = delete;

        std::string GetFontKey( FontType type, FontSize size ) const;
        float       GetFontSizeValue( FontSize size ) const;

    private:
        std::unordered_map<std::string, ImFont *> m_fonts;
        ImFont                                   *m_defaultFont = nullptr;
        FontConfig                                m_config;
        bool                                      m_initialized = false;
    };

    // FontAwesome 6 Free icon codes
    namespace Icons
    {
        constexpr auto FOLDER      = "\uf07b";
        constexpr auto FOLDER_OPEN = "\uf07c";
        constexpr auto FILE_ICON   = "\uf15b";
        constexpr auto IMAGE       = "\uf03e";
        constexpr auto AUDIO       = "\uf001";
        constexpr auto VIDEO       = "\uf008";
        constexpr auto CODE        = "\uf121";
        constexpr auto MESH        = "\uf1b2";
        constexpr auto TEXTURE     = "\uf03e";
        constexpr auto MATERIAL    = "\uf1fc";
        constexpr auto PREFAB      = "\uf1b3";
        constexpr auto SCENE       = "\uf0ac";
        constexpr auto SCRIPT      = "\uf121";
        constexpr auto SHADER      = "\uf72b";
        constexpr auto REFRESH     = "\uf021";
        constexpr auto ARROW_LEFT  = "\uf060";
        constexpr auto ARROW_RIGHT = "\uf061";
        constexpr auto HOME        = "\uf015";
        constexpr auto SEARCH      = "\uf002";
        constexpr auto PLUS        = "\uf067";
        constexpr auto MINUS       = "\uf068";
        constexpr auto SETTINGS    = "\uf013";
        constexpr auto PLAY        = "\uf04b";
        constexpr auto PAUSE       = "\uf04c";
        constexpr auto STOP        = "\uf04d";
    } // namespace Icons
} // namespace DZEngine
