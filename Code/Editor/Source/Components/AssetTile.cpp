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

#include "DZEngine/Components/AssetTile.h"
#include <algorithm>
#include <sstream>
#include <vector>
#include "DZEngine/Components/AssetBrowser.h"
#include "DZEngine/Style/ImGuiFonts.h"

using namespace DZEngine;

bool AssetTile::Render( const AssetItem &item, int tileSize, bool isSelected, int uniqueId, const Style &style )
{
    ImGui::PushID( uniqueId );

    bool         clicked = false;
    const ImVec2 totalTileSize( static_cast<float>( tileSize ), static_cast<float>( tileSize ) );

    if ( ImGui::InvisibleButton( "##tile", totalTileSize ) )
    {
        clicked = true;
    }

    bool isHovered = ImGui::IsItemHovered( );
    bool isActive  = ImGui::IsItemActive( );

    ImDrawList  *drawList = ImGui::GetWindowDrawList( );
    const ImVec2 itemMin  = ImGui::GetItemRectMin( );
    const ImVec2 itemMax  = ImGui::GetItemRectMax( );

    if ( isSelected || isHovered || isActive )
    {
        ImVec4 bgColor = isSelected ? style.selectedColor : ( isActive ? style.selectedColor : style.hoveredColor );
        drawList->AddRectFilled( itemMin, itemMax, ImGui::ColorConvertFloat4ToU32( bgColor ), style.borderRadius );

        if ( isSelected )
        {
            drawList->AddRect( itemMin, itemMax, ImGui::ColorConvertFloat4ToU32( ImVec4( 0.4f, 0.6f, 1.0f, 0.8f ) ), style.borderRadius, 0, 2.0f );
        }
    }

    const float iconAreaHeight = totalTileSize.y * 0.7f;
    const float textAreaHeight = totalTileSize.y * 0.3f;

    const char    *icon         = GetFileIcon( item );
    const ImVec4   iconColor    = GetFileIconColor( item );
    const FontSize iconFontSize = GetIconFontSize( tileSize );

    ImGuiFonts::Instance( ).PushIconFont( iconFontSize );
    ImFont      *iconFont       = ImGui::GetFont( );
    const float  actualIconSize = iconFont->FontSize;
    const ImVec2 iconTextSize   = iconFont->CalcTextSizeA( actualIconSize, FLT_MAX, 0.0f, icon );

    const auto iconPos = ImVec2( itemMin.x + ( totalTileSize.x - iconTextSize.x ) * 0.5f, itemMin.y + ( iconAreaHeight - iconTextSize.y ) * 0.5f );

    drawList->AddText( iconFont, actualIconSize, iconPos, ImGui::ColorConvertFloat4ToU32( iconColor ), icon );
    ImGuiFonts::Instance( ).PopIconFont( );

    ImGuiFonts::Instance( ).PushFont( FontType::Regular, style.textFontSize );
    ImFont     *textFont        = ImGui::GetFont( );
    const float lineHeight      = ImGui::GetTextLineHeight( );
    const float actualTextWidth = totalTileSize.x - style.textPadding * 2;

    std::vector<std::string> words;
    std::istringstream       iss( item.name );
    std::string              word;
    while ( iss >> word )
    {
        words.push_back( word );
    }

    std::vector<std::string> lines;
    std::string              currentLine = "";
    for ( const auto &w : words )
    {
        std::string testLine = currentLine.empty( ) ? w : currentLine + " " + w;
        if ( float testWidth = textFont->CalcTextSizeA( textFont->FontSize, FLT_MAX, 0.0f, testLine.c_str( ) ).x; testWidth <= actualTextWidth )
        {
            currentLine = testLine;
        }
        else
        {
            if ( !currentLine.empty( ) )
            {
                lines.push_back( currentLine );
                currentLine = w;

                // Check if a single word text is too long then split it up
                if ( float singleWordWidth = textFont->CalcTextSizeA( textFont->FontSize, FLT_MAX, 0.0f, w.c_str( ) ).x; singleWordWidth > actualTextWidth )
                {
                    currentLine          = "";
                    std::string charLine = "";
                    for ( char c : w )
                    {
                        std::string testChar = charLine + c;
                        if ( float charWidth = textFont->CalcTextSizeA( textFont->FontSize, FLT_MAX, 0.0f, testChar.c_str( ) ).x; charWidth <= actualTextWidth )
                        {
                            charLine = testChar;
                        }
                        else
                        {
                            if ( !charLine.empty( ) )
                            {
                                lines.push_back( charLine );
                            }
                            charLine = c;
                        }
                    }
                    if ( !charLine.empty( ) )
                    {
                        currentLine = charLine;
                    }
                }
            }
            else
            {
                std::string charLine = "";
                for ( char c : w )
                {
                    std::string testChar  = charLine + c;
                    float       charWidth = textFont->CalcTextSizeA( textFont->FontSize, FLT_MAX, 0.0f, testChar.c_str( ) ).x;
                    if ( charWidth <= actualTextWidth )
                    {
                        charLine = testChar;
                    }
                    else
                    {
                        if ( !charLine.empty( ) )
                        {
                            lines.push_back( charLine );
                        }
                        charLine = c;
                    }
                }
                if ( !charLine.empty( ) )
                {
                    currentLine = charLine;
                }
            }
        }
    }
    if ( !currentLine.empty( ) )
    {
        lines.push_back( currentLine );
    }

    // wrap only 2 lines
    if ( lines.size( ) > 2 )
    {
        lines.resize( 2 );
        std::string &lastLine = lines.back( );
        while ( lastLine.length( ) > 3 )
        {
            std::string testEllipsis = lastLine + "...";
            if ( float ellipsisWidth = textFont->CalcTextSizeA( textFont->FontSize, FLT_MAX, 0.0f, testEllipsis.c_str( ) ).x; ellipsisWidth <= actualTextWidth )
            {
                break;
            }
            lastLine.pop_back( );
        }
        if ( lastLine.length( ) > 0 )
        {
            lastLine += "...";
        }
    }

    const float textStartY         = itemMin.y + iconAreaHeight + style.textPadding;
    const float totalTextHeight    = lines.size( ) * lineHeight;
    const float textVerticalCenter = textStartY + ( textAreaHeight - style.textPadding * 2 - totalTextHeight ) * 0.5f;

    for ( size_t i = 0; i < lines.size( ); ++i )
    {
        const float lineWidth = textFont->CalcTextSizeA( textFont->FontSize, FLT_MAX, 0.0f, lines[ i ].c_str( ) ).x;
        const float lineX     = itemMin.x + ( totalTileSize.x - lineWidth ) * 0.5f; // Center each line
        const float lineY     = textVerticalCenter + i * lineHeight;

        drawList->AddText( textFont, textFont->FontSize, ImVec2( lineX, lineY ), ImGui::ColorConvertFloat4ToU32( style.textColor ), lines[ i ].c_str( ) );
    }

    ImGuiFonts::Instance( ).PopFont( );
    ImGui::PopID( );

    return clicked;
}

// TODO these shouldn't be hardcoded
const char *AssetTile::GetFileIcon( const AssetItem &item )
{
    if ( item.isDirectory )
    {
        return Icons::FOLDER;
    }

    std::string ext = item.extension;
    std::ranges::transform( ext, ext.begin( ), ::tolower );

    if ( ext == "png" || ext == "jpg" || ext == "jpeg" || ext == "bmp" || ext == "tga" || ext == "dds" )
    {
        return Icons::IMAGE;
    }
    if ( ext == "wav" || ext == "mp3" || ext == "ogg" || ext == "flac" )
    {
        return Icons::AUDIO;
    }
    if ( ext == "mp4" || ext == "avi" || ext == "mov" || ext == "mkv" )
    {
        return Icons::VIDEO;
    }
    if ( ext == "cpp" || ext == "h" || ext == "cs" || ext == "js" || ext == "py" )
    {
        return Icons::CODE;
    }
    if ( ext == "obj" || ext == "fbx" || ext == "dae" || ext == "blend" )
    {
        return Icons::MESH;
    }
    if ( ext == "hlsl" || ext == "glsl" || ext == "shader" )
    {
        return Icons::SHADER;
    }
    return Icons::FILE_ICON;
}

// TODO these shouldn't be hardcoded
ImVec4 AssetTile::GetFileIconColor( const AssetItem &item )
{
    if ( item.isDirectory )
    {
        return ImVec4( 1.0f, 0.8f, 0.2f, 1.0f ); // Golden yellow for folders
    }

    std::string ext = item.extension;
    std::ranges::transform( ext, ext.begin( ), ::tolower );

    if ( ext == "png" || ext == "jpg" || ext == "jpeg" || ext == "bmp" || ext == "tga" || ext == "dds" )
    {
        return ImVec4( 0.2f, 0.8f, 0.2f, 1.0f ); // Green for images
    }
    if ( ext == "wav" || ext == "mp3" || ext == "ogg" || ext == "flac" )
    {
        return ImVec4( 0.8f, 0.2f, 0.8f, 1.0f ); // Magenta for audio
    }
    if ( ext == "mp4" || ext == "avi" || ext == "mov" || ext == "mkv" )
    {
        return ImVec4( 0.8f, 0.4f, 0.2f, 1.0f ); // Orange for video
    }
    if ( ext == "cpp" || ext == "h" || ext == "cs" || ext == "js" || ext == "py" )
    {
        return ImVec4( 0.2f, 0.6f, 0.9f, 1.0f ); // Blue for code
    }
    if ( ext == "obj" || ext == "fbx" || ext == "dae" || ext == "blend" )
    {
        return ImVec4( 0.6f, 0.2f, 0.8f, 1.0f ); // Purple for 3D models
    }
    if ( ext == "hlsl" || ext == "glsl" || ext == "shader" )
    {
        return ImVec4( 0.8f, 0.6f, 0.2f, 1.0f ); // Gold for shaders
    }
    return ImVec4( 0.7f, 0.7f, 0.7f, 1.0f ); // Gray for unknown files
}

FontSize AssetTile::GetIconFontSize( const int tileSize )
{
    if ( tileSize <= 64 )
    {
        return FontSize::Medium;
    }
    if ( tileSize <= 96 )
    {
        return FontSize::Large;
    }
    if ( tileSize <= 128 )
    {
        return FontSize::VeryLarge;
    }
    if ( tileSize <= 160 )
    {
        return FontSize::VeryLarge;
    }
    return FontSize::VeryLarge;
}
