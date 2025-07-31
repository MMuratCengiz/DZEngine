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

#include "DZEngine/Components/AssetBrowser.h"
#include <algorithm>
#include <imgui_internal.h>
#include <iomanip>
#include <sstream>
#include "DZEngine/Style/EditorStyle.h"
#include "DZEngine/Style/ImGuiFonts.h"

using namespace DZEngine;

AssetBrowser::AssetBrowser( ) :
    m_rootPath( "./build" ), m_currentPath( "./build" ), m_historyIndex( -1 ), m_viewSize( ViewSize::Medium ), m_itemsPerRow( 5 ), m_selectedItem( -1 ), m_showContextMenu( false ),
    m_showHiddenFiles( false ), m_isOpen( true )
{
    if ( std::filesystem::exists( m_rootPath ) )
    {
        m_currentPath = std::filesystem::canonical( m_rootPath ).string( );
        m_rootPath    = m_currentPath;
    }
    else
    {
        m_rootPath    = std::filesystem::current_path( ).string( );
        m_currentPath = m_rootPath;
    }

    m_navigationHistory.push_back( m_currentPath );
    m_historyIndex = 0;
    ScanCurrentDirectory( );
}

AssetBrowser::~AssetBrowser( )
{
}

void AssetBrowser::Render( )
{
    if ( !m_isOpen )
    {
        return;
    }

    ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 0, 0 ) );
    if ( ImGui::Begin( "Asset Browser", &m_isOpen ) )
    {
        ImGui::PopStyleVar( );

        RenderToolbar( );
        RenderNavigationBar( );

        ImGui::Separator( );

        RenderAssetGrid( );

        if ( m_showContextMenu )
        {
            RenderContextMenu( );
        }
    }
    else
    {
        ImGui::PopStyleVar( );
    }
    ImGui::End( );
}

void AssetBrowser::SetRootPath( const std::string &path )
{
    if ( IsValidPath( path ) )
    {
        m_rootPath = std::filesystem::canonical( path ).string( );
        NavigateToPath( m_rootPath );
    }
}

void AssetBrowser::NavigateToPath( const std::string &path )
{
    if ( !IsValidPath( path ) )
    {
        return;
    }
    if ( const std::string canonicalPath = std::filesystem::canonical( path ).string( ); canonicalPath != m_currentPath )
    {
        m_currentPath = canonicalPath;

        if ( m_historyIndex == -1 || m_navigationHistory[ m_historyIndex ] != canonicalPath )
        {
            m_navigationHistory.erase( m_navigationHistory.begin( ) + m_historyIndex + 1, m_navigationHistory.end( ) );
            m_navigationHistory.push_back( canonicalPath );
            m_historyIndex = static_cast<int>( m_navigationHistory.size( ) ) - 1;
        }

        ScanCurrentDirectory( );
    }
}

void AssetBrowser::RefreshCurrentDirectory( )
{
    ScanCurrentDirectory( );
}

void AssetBrowser::RenderToolbar( )
{
    EditorStyle::BeginToolbar( );
    ImGuiFonts::Instance( ).PushFont( FontType::Regular, FontSize::Medium );

    if ( ImGui::Button( Icons::ARROW_LEFT ) )
    {
        NavigateBack( );
    }
    ImGui::SameLine( );

    if ( ImGui::Button( Icons::HOME ) )
    {
        GoHome( );
    }
    ImGui::SameLine( );

    if ( ImGui::Button( Icons::REFRESH ) )
    {
        RefreshCurrentDirectory( );
    }

    ImGuiFonts::Instance( ).PopFont( );
    ImGui::SameLine( );

    ImGui::SeparatorEx( ImGuiSeparatorFlags_Vertical );
    ImGui::SameLine( );

    ImGui::SetNextItemWidth( 200 );
    char searchBuffer[ 256 ];
    strncpy( searchBuffer, m_searchFilter.c_str( ), sizeof( searchBuffer ) - 1 );
    searchBuffer[ sizeof( searchBuffer ) - 1 ] = '\0';
    if ( ImGui::InputTextWithHint( "##Search", "Search...", searchBuffer, sizeof( searchBuffer ) ) )
    {
        m_searchFilter = searchBuffer;
        ScanCurrentDirectory( );
    }
    ImGui::SameLine( );

    ImGui::SetNextItemWidth( 120 );
    int currentSize = static_cast<int>( m_viewSize );
    if ( ImGui::Combo( "##ViewSize", &currentSize, VIEW_SIZE_NAMES, 5 ) )
    {
        m_viewSize = static_cast<ViewSize>( currentSize );
    }

    EditorStyle::EndToolbar( );
}

void AssetBrowser::RenderNavigationBar( )
{
    ImGuiFonts::Instance( ).PushFont( FontType::Regular, FontSize::Small );
    ImGui::PushStyleVar( ImGuiStyleVar_FramePadding, ImVec2( 4, 2 ) );
    ImGui::PushStyleColor( ImGuiCol_Button, ImVec4( 0, 0, 0, 0 ) );

    std::string displayPath = m_currentPath;
    std::ranges::replace( displayPath, '\\', '/' );

    std::vector<std::string> pathParts;
    std::stringstream        ss( displayPath );
    std::string              part;

    while ( std::getline( ss, part, '/' ) )
    {
        if ( !part.empty( ) )
        {
            pathParts.push_back( part );
        }
    }

    for ( size_t i = 0; i < pathParts.size( ); ++i )
    {
        if ( i > 0 )
        {
            ImGui::SameLine( );
            ImGui::Text( "/" );
            ImGui::SameLine( );
        }

        if ( ImGui::Button( pathParts[ i ].c_str( ) ) )
        {
            std::string targetPath;
            for ( size_t j = 0; j <= i; ++j )
            {
                targetPath += "/" + pathParts[ j ];
            }
            NavigateToPath( targetPath );
        }
    }

    ImGui::PopStyleColor( );
    ImGui::PopStyleVar( );
    ImGuiFonts::Instance( ).PopFont( );
}

void AssetBrowser::RenderAssetGrid( )
{
    ImGui::BeginChild( "AssetGrid", ImVec2( 0, 0 ), false, ImGuiWindowFlags_NoScrollWithMouse );

    const float windowWidth = ImGui::GetContentRegionAvail( ).x;
    const int   itemSize    = ITEM_SIZES[ static_cast<int>( m_viewSize ) ];
    m_itemsPerRow           = std::max( 1, static_cast<int>( windowWidth / itemSize ) );

    for ( int i = 0; i < static_cast<int>( m_items.size( ) ); ++i )
    {
        if ( i % m_itemsPerRow != 0 )
        {
            ImGui::SameLine( );
        }

        RenderAssetItem( m_items[ i ], i );
    }

    if ( ImGui::IsWindowHovered( ) && ImGui::IsMouseClicked( ImGuiMouseButton_Right ) )
    {
        m_showContextMenu = true;
        ImGui::OpenPopup( "AssetContextMenu" );
    }

    ImGui::EndChild( );
}

void AssetBrowser::RenderAssetItem( const AssetItem &item, int index )
{
    ImGui::BeginGroup( );

    const bool   isSelected = ( m_selectedItem == index );
    const int    itemSize   = ITEM_SIZES[ static_cast<int>( m_viewSize ) ];
    const ImVec2 buttonSize( static_cast<float>( itemSize ), static_cast<float>( itemSize ) );

    if ( isSelected )
    {
        ImGui::PushStyleColor( ImGuiCol_Button, ImVec4( 0.3f, 0.5f, 0.8f, 0.8f ) );
    }
    else
    {
        ImGui::PushStyleColor( ImGuiCol_Button, ImVec4( 0.2f, 0.2f, 0.2f, 0.8f ) );
    }

    ImGui::PushStyleColor( ImGuiCol_ButtonHovered, ImVec4( 0.3f, 0.3f, 0.3f, 0.9f ) );
    ImGui::PushStyleColor( ImGuiCol_ButtonActive, ImVec4( 0.4f, 0.4f, 0.4f, 1.0f ) );

    if ( ImGui::Button( ( "##item" + std::to_string( index ) ).c_str( ), buttonSize ) )
    {
        m_selectedItem = index;

        if ( item.isDirectory )
        {
            NavigateToPath( item.path );
        }
    }

    if ( ImGui::IsItemHovered( ) && ImGui::IsMouseDoubleClicked( ImGuiMouseButton_Left ) )
    {
        if ( item.isDirectory )
        {
            NavigateToPath( item.path );
        }
    }

    ImGui::PopStyleColor( 3 );

    ImDrawList  *drawList  = ImGui::GetWindowDrawList( );
    const ImVec2 buttonMin = ImGui::GetItemRectMin( );
    const ImVec2 buttonMax = ImGui::GetItemRectMax( );

    const char  *icon      = GetFileIcon( item );
    const ImVec4 iconColor = GetFileColor( item );

    FontSize fontSize;
    switch ( m_viewSize )
    {
    case ViewSize::VerySmall:
        fontSize = FontSize::VerySmall;
        break;
    case ViewSize::Small:
        fontSize = FontSize::Small;
        break;
    case ViewSize::Medium:
        fontSize = FontSize::Medium;
        break;
    case ViewSize::Large:
        fontSize = FontSize::Large;
        break;
    case ViewSize::VeryLarge:
        fontSize = FontSize::VeryLarge;
        break;
    }

    ImGuiFonts::Instance( ).PushFont( FontType::Regular, fontSize );
    ImFont      *font         = ImGui::GetFont( );
    const ImVec2 iconTextSize = font->CalcTextSizeA( font->FontSize, FLT_MAX, 0.0f, icon );

    const auto iconPos = ImVec2( buttonMin.x + ( buttonSize.x - iconTextSize.x ) * 0.5f, buttonMin.y + ( buttonSize.y - iconTextSize.y ) * 0.5f - 8 );

    drawList->AddText( font, font->FontSize, iconPos, ImGui::ColorConvertFloat4ToU32( iconColor ), icon );
    ImGuiFonts::Instance( ).PopFont( );

    FontSize textFontSize;
    switch ( m_viewSize )
    {
    case ViewSize::VerySmall:
        textFontSize = FontSize::VerySmall;
        break;
    case ViewSize::Small:
    case ViewSize::Medium:
        textFontSize = FontSize::Small;
        break;
    case ViewSize::Large:
        textFontSize = FontSize::Medium;
        break;
    case ViewSize::VeryLarge:
        textFontSize = FontSize::Large;
        break;
    }

    ImGuiFonts::Instance( ).PushFont( FontType::Regular, textFontSize );
    ImFont *textFont = ImGui::GetFont( );

    ImVec2      textSize    = textFont->CalcTextSizeA( textFont->FontSize, FLT_MAX, 0.0f, item.name.c_str( ) );
    std::string displayName = item.name;

    if ( const float maxTextWidth = buttonSize.x - 8; textSize.x > maxTextWidth )
    {
        while ( textSize.x > maxTextWidth && displayName.length( ) > 3 )
        {
            displayName = displayName.substr( 0, displayName.length( ) - 4 ) + "...";
            textSize    = textFont->CalcTextSizeA( textFont->FontSize, FLT_MAX, 0.0f, displayName.c_str( ) );
        }
    }

    const auto textPos = ImVec2( buttonMin.x + ( buttonSize.x - textSize.x ) * 0.5f, buttonMax.y + 4 );

    drawList->AddText( textFont, textFont->FontSize, textPos, ImGui::ColorConvertFloat4ToU32( ImVec4( 0.9f, 0.9f, 0.9f, 1.0f ) ), displayName.c_str( ) );
    ImGuiFonts::Instance( ).PopFont( );

    ImGui::EndGroup( );
}

void AssetBrowser::RenderContextMenu( )
{
    if ( ImGui::BeginPopup( "AssetContextMenu" ) )
    {
        if ( ImGui::MenuItem( "Refresh" ) )
        {
            RefreshCurrentDirectory( );
        }

        ImGui::Separator( );

        if ( ImGui::MenuItem( "Show Hidden Files", nullptr, m_showHiddenFiles ) )
        {
            m_showHiddenFiles = !m_showHiddenFiles;
            ScanCurrentDirectory( );
        }

        ImGui::EndPopup( );
    }
    else
    {
        m_showContextMenu = false;
    }
}

const char *AssetBrowser::GetFileIcon( const AssetItem &item )
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

ImVec4 AssetBrowser::GetFileColor( const AssetItem &item )
{
    if ( item.isDirectory )
    {
        return ImVec4( 1.0f, 0.8f, 0.2f, 1.0f );
    }

    std::string ext = item.extension;
    std::ranges::transform( ext, ext.begin( ), ::tolower );

    if ( ext == "png" || ext == "jpg" || ext == "jpeg" || ext == "bmp" || ext == "tga" || ext == "dds" )
    {
        return ImVec4( 0.2f, 0.8f, 0.2f, 1.0f );
    }
    if ( ext == "wav" || ext == "mp3" || ext == "ogg" || ext == "flac" )
    {
        return ImVec4( 0.8f, 0.2f, 0.8f, 1.0f );
    }
    if ( ext == "cpp" || ext == "h" || ext == "cs" || ext == "js" || ext == "py" )
    {
        return ImVec4( 0.2f, 0.6f, 0.9f, 1.0f );
    }

    return ImVec4( 0.7f, 0.7f, 0.7f, 1.0f );
}

std::string AssetBrowser::FormatFileSize( size_t bytes )
{
    const char *suffixes[]  = { "B", "KB", "MB", "GB", "TB" };
    int         suffixIndex = 0;
    double      size        = static_cast<double>( bytes );

    while ( size >= 1024 && suffixIndex < 4 )
    {
        size /= 1024;
        suffixIndex++;
    }

    std::ostringstream oss;
    oss << std::fixed << std::setprecision( 1 ) << size << " " << suffixes[ suffixIndex ];
    return oss.str( );
}

void AssetBrowser::NavigateUp( )
{
    if ( const std::filesystem::path currentPath( m_currentPath ); currentPath.has_parent_path( ) )
    {
        NavigateToPath( currentPath.parent_path( ).string( ) );
    }
}

void AssetBrowser::NavigateBack( )
{
    if ( m_historyIndex > 0 )
    {
        m_historyIndex--;
        m_currentPath = m_navigationHistory[ m_historyIndex ];
        ScanCurrentDirectory( );
    }
}

void AssetBrowser::NavigateForward( )
{
    if ( m_historyIndex < static_cast<int>( m_navigationHistory.size( ) ) - 1 )
    {
        m_historyIndex++;
        m_currentPath = m_navigationHistory[ m_historyIndex ];
        ScanCurrentDirectory( );
    }
}

void AssetBrowser::GoHome( )
{
    NavigateToPath( m_rootPath );
}

void AssetBrowser::ScanCurrentDirectory( )
{
    m_items.clear( );
    m_selectedItem = -1;

    if ( !IsValidPath( m_currentPath ) )
    {
        return;
    }

    for ( const auto &entry : std::filesystem::directory_iterator( m_currentPath ) )
    {
        std::string filename = entry.path( ).filename( ).string( );

        if ( !m_showHiddenFiles && filename[ 0 ] == '.' )
        {
            continue;
        }

        if ( !m_searchFilter.empty( ) )
        {
            std::string lowerFilename = filename;
            std::string lowerFilter   = m_searchFilter;
            std::ranges::transform( lowerFilename, lowerFilename.begin( ), ::tolower );
            std::ranges::transform( lowerFilter, lowerFilter.begin( ), ::tolower );

            if ( lowerFilename.find( lowerFilter ) == std::string::npos )
            {
                continue;
            }
        }

        AssetItem item( filename, entry.path( ).string( ), entry.is_directory( ) );
        if ( !entry.is_directory( ) )
        {
            item.fileSize = std::filesystem::file_size( entry.path( ) );
        }
        m_items.push_back( item );
    }

    std::ranges::sort( m_items,
                       []( const AssetItem &a, const AssetItem &b )
                       {
                           if ( a.isDirectory != b.isDirectory )
                           {
                               return a.isDirectory > b.isDirectory;
                           }
                           return a.name < b.name;
                       } );
}

bool AssetBrowser::IsValidPath( const std::string &path )
{
    return std::filesystem::exists( path ) && std::filesystem::is_directory( path );
}
