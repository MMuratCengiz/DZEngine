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
#include <imgui_stdlib.h>
#include <iomanip>
#include <sstream>
#include "DZEngine/Components/AssetTile.h"
#include "DZEngine/Style/EditorStyle.h"
#include "DZEngine/Style/ImGuiFonts.h"

using namespace DZEngine;

AssetBrowser::AssetBrowser( ) :
    m_rootPath( "./build" ), m_currentPath( "./build" ), m_historyIndex( -1 ), m_viewSize( ViewSize::VeryLarge ), m_itemsPerRow( 5 ), m_selectedItem( -1 ),
    m_showContextMenu( false ), m_showHiddenFiles( false ), m_isOpen( true )
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

        ImGui::Dummy( ImVec2( 0, 4.0f ) );

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

    constexpr float margin = 16.0f;
    ImGui::SetCursorPosX( ImGui::GetCursorPosX( ) + margin );

    ImGuiFonts::Instance( ).PushIconFont( FontSize::VerySmall );
    const float iconButtonHeight = ImGui::GetFrameHeight( );

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
    ImGuiFonts::Instance( ).PopIconFont( );

    ImGuiFonts::Instance( ).PushFont( FontType::Regular, FontSize::VerySmall );
    const float textFrameHeight = ImGui::GetFrameHeight( );

    ImGui::SameLine( );
    const float heightDiff = ( iconButtonHeight - textFrameHeight ) * 0.5f;
    if ( heightDiff > 0 )
    {
        ImGui::SetCursorPosY( ImGui::GetCursorPosY( ) + heightDiff );
    }

    constexpr float itemWidth = 250.0f;
    ImGui::SetNextItemWidth( itemWidth );
    ImGui::SetCursorPosX( ImGui::GetCursorPosX( ) + margin );
    if ( ImGui::InputTextWithHint( "##Search", "Search...", &m_searchFilter ) )
    {
        ScanCurrentDirectory( );
    }

    ImGui::SameLine( );
    if ( heightDiff > 0 )
    {
        ImGui::SetCursorPosY( ImGui::GetCursorPosY( ) + heightDiff );
    }
    ImGui::SetNextItemWidth( itemWidth );
    ImGui::SetCursorPosX( ImGui::GetCursorPosX( ) + margin );
    int currentSize = static_cast<int>( m_viewSize );
    if ( ImGui::Combo( "##ViewSize", &currentSize, VIEW_SIZE_NAMES, 5 ) )
    {
        m_viewSize = static_cast<ViewSize>( currentSize );
    }

    ImGuiFonts::Instance( ).PopFont( );
    EditorStyle::EndToolbar( );
}

void AssetBrowser::RenderNavigationBar( )
{
    ImGuiFonts::Instance( ).PushFont( FontType::Regular, FontSize::VerySmall );
    const float navBarHeight = ImGui::GetFrameHeight( );
    ImGuiFonts::Instance( ).PopFont( );

    ImGui::BeginChild( "##NavigationBar", ImVec2( 0, navBarHeight ), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_HorizontalScrollbar );

    ImGuiFonts::Instance( ).PushFont( FontType::Regular, FontSize::VerySmall );
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
            ImGui::TextUnformatted( "/" );
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
    ImGui::PopFont( );

    ImGui::EndChild( );
}

void AssetBrowser::RenderAssetGrid( )
{
    ImGui::BeginChild( "AssetGrid", ImVec2( 0, 0 ), false, ImGuiWindowFlags_NoScrollWithMouse );

    const float     windowWidth = ImGui::GetContentRegionAvail( ).x;
    const int       itemSize    = ITEM_SIZES[ static_cast<int>( m_viewSize ) ];
    constexpr float spacing     = 8.0f;
    constexpr float padding     = 12.0f;

    const float availableWidth = windowWidth - padding * 2;
    m_itemsPerRow              = std::max( 1, static_cast<int>( ( availableWidth + spacing ) / ( itemSize + spacing ) ) );

    ImGui::SetCursorPosX( ImGui::GetCursorPosX( ) + padding );

    for ( int i = 0; i < static_cast<int>( m_items.size( ) ); ++i )
    {
        if ( i % m_itemsPerRow != 0 )
        {
            ImGui::SameLine( 0, spacing );
        }
        else if ( i > 0 )
        {
            ImGui::Dummy( ImVec2( 0, spacing * 0.5f ) );
            ImGui::SetCursorPosX( ImGui::GetCursorPosX( ) + padding );
        }
        if ( AssetTile::Render( m_items[ i ], ITEM_SIZES[ static_cast<int>( m_viewSize ) ], m_selectedItem == i, i ) )
        {
            m_selectedItem = i;
        }
        if ( ImGui::IsItemHovered( ) && ImGui::IsMouseDoubleClicked( ImGuiMouseButton_Left ) )
        {
            if ( m_items[ i ].isDirectory )
            {
                NavigateToPath( m_items[ i ].path );
            }
        }
    }

    ImGui::Dummy( ImVec2( 0, padding ) );
    if ( ImGui::IsWindowHovered( ) && ImGui::IsMouseClicked( ImGuiMouseButton_Right ) )
    {
        m_showContextMenu = true;
        ImGui::OpenPopup( "AssetContextMenu" );
    }

    ImGui::EndChild( );
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

std::string AssetBrowser::FormatFileSize( const size_t bytes )
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
