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

#include "DZEngine/Style/ImGuiFonts.h"

#include <sstream>

#include "DenOfIzGraphics/Input/Display.h"

extern unsigned int  JetbrainsMonoImGui_size;
extern unsigned char JetbrainsMonoImGui_data[];

extern unsigned int  FontAwesome_size;
extern unsigned char FontAwesome_data[];

using namespace DZEngine;

ImGuiFonts &ImGuiFonts::Instance( )
{
    static ImGuiFonts instance;
    return instance;
}

void ImGuiFonts::Initialize( const FontConfig &config )
{
    constexpr float dpiScale = 1.5f; // Todo
    m_config                 = config;
    m_config.VerySmallSize *= dpiScale;
    m_config.SmallSize *= dpiScale;
    m_config.MediumSize *= dpiScale;
    m_config.LargeSize *= dpiScale;
    m_config.VeryLargeSize *= dpiScale;
    m_initialized = true;
}

void ImGuiFonts::LoadFonts( )
{
    if ( !m_initialized )
    {
        Initialize( );
    }

    const ImGuiIO &io = ImGui::GetIO( );
    io.Fonts->Clear( );
    m_fonts.clear( );

    static constexpr ImWchar icon_ranges[] = { 0xe000, 0xf8ff, 0 };
    {
        ImFontConfig fontConfig;
        fontConfig.FontDataOwnedByAtlas = false;

        auto *fontVerySmall = io.Fonts->AddFontFromMemoryTTF( JetbrainsMonoImGui_data, JetbrainsMonoImGui_size, m_config.VerySmallSize, &fontConfig );
        m_fonts[ GetFontKey( FontType::Regular, FontSize::VerySmall ) ] = fontVerySmall;

        fontConfig.MergeMode = false;
        auto *fontSmall      = io.Fonts->AddFontFromMemoryTTF( JetbrainsMonoImGui_data, JetbrainsMonoImGui_size, m_config.SmallSize, &fontConfig );
        m_fonts[ GetFontKey( FontType::Regular, FontSize::Small ) ] = fontSmall;

        fontConfig.MergeMode = false;
        auto *fontMedium     = io.Fonts->AddFontFromMemoryTTF( JetbrainsMonoImGui_data, JetbrainsMonoImGui_size, m_config.MediumSize, &fontConfig );
        m_fonts[ GetFontKey( FontType::Regular, FontSize::Medium ) ] = fontMedium;
        m_defaultFont                                                = fontMedium;

        fontConfig.MergeMode = false;
        auto *fontLarge      = io.Fonts->AddFontFromMemoryTTF( JetbrainsMonoImGui_data, JetbrainsMonoImGui_size, m_config.LargeSize, &fontConfig );
        m_fonts[ GetFontKey( FontType::Regular, FontSize::Large ) ] = fontLarge;

        fontConfig.MergeMode = false;
        auto *fontVeryLarge  = io.Fonts->AddFontFromMemoryTTF( JetbrainsMonoImGui_data, JetbrainsMonoImGui_size, m_config.VeryLargeSize, &fontConfig );
        m_fonts[ GetFontKey( FontType::Regular, FontSize::VeryLarge ) ] = fontVeryLarge;
    }

    ImFontConfig iconConfig;
    iconConfig.FontDataOwnedByAtlas = false;

    const float        iconSizes[] = { m_config.VerySmallSize, m_config.SmallSize, m_config.MediumSize, m_config.LargeSize, m_config.VeryLargeSize };
    constexpr FontSize fontSizes[] = { FontSize::VerySmall, FontSize::Small, FontSize::Medium, FontSize::Large, FontSize::VeryLarge };

    for ( int i = 0; i < 5; ++i )
    {
        iconConfig.SizePixels                                   = iconSizes[ i ];
        auto *iconFont                                          = io.Fonts->AddFontFromMemoryTTF( FontAwesome_data, FontAwesome_size, iconSizes[ i ], &iconConfig, icon_ranges );
        m_fonts[ GetFontKey( FontType::Icon, fontSizes[ i ] ) ] = iconFont;
    }

    io.Fonts->Build( );
}

ImFont *ImGuiFonts::GetFont( const FontType type, const FontSize size )
{
    const auto key = GetFontKey( type, size );
    const auto it  = m_fonts.find( key );
    return ( it != m_fonts.end( ) ) ? it->second : m_defaultFont;
}

ImFont *ImGuiFonts::GetIconFont( const FontSize size )
{
    return GetFont( FontType::Icon, size );
}

void ImGuiFonts::PushFont( const FontType type, const FontSize size )
{
    if ( auto *font = GetFont( type, size ) )
    {
        ImGui::PushFont( font );
    }
}

void ImGuiFonts::PopFont( ) const
{
    ImGui::PopFont( );
}

void ImGuiFonts::PushIconFont( const FontSize size )
{
    PushFont( FontType::Icon, size );
}

void ImGuiFonts::PopIconFont( ) const
{
    PopFont( );
}

std::string ImGuiFonts::GetFontKey( FontType type, FontSize size ) const
{
    std::stringstream ss;
    ss << static_cast<int>( type ) << "_" << static_cast<int>( size );
    return ss.str( );
}

float ImGuiFonts::GetFontSizeValue( const FontSize size ) const
{
    switch ( size )
    {
    case FontSize::VerySmall:
        return m_config.VerySmallSize;
    case FontSize::Small:
        return m_config.SmallSize;
    case FontSize::Medium:
        return m_config.MediumSize;
    case FontSize::Large:
        return m_config.LargeSize;
    case FontSize::VeryLarge:
        return m_config.VeryLargeSize;
    default:
        return m_config.MediumSize;
    }
}
ImFont *ImGuiFonts::GetDefaultFont( ) const
{
    return m_defaultFont;
}
