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

#include <filesystem>
#include <imgui.h>
#include <string>
#include <vector>

namespace DZEngine
{
    struct AssetItem
    {
        std::string name;
        std::string path;
        bool        isDirectory;
        std::string extension;
        size_t      fileSize;

        AssetItem( const std::string &itemName, const std::string &itemPath, const bool isDir ) : name( itemName ), path( itemPath ), isDirectory( isDir ), fileSize( 0 )
        {
            if ( !isDirectory )
            {
                if ( const auto pos = name.find_last_of( '.' ); pos != std::string::npos )
                {
                    extension = name.substr( pos + 1 );
                }
            }
        }
    };

    class AssetBrowser
    {
        std::string              m_rootPath;
        std::string              m_currentPath;
        std::vector<std::string> m_navigationHistory;
        int                      m_historyIndex;

        std::vector<AssetItem> m_items;
        std::string            m_searchFilter;

        enum class ViewSize
        {
            VerySmall = 0,
            Small     = 1,
            Medium    = 2,
            Large     = 3,
            VeryLarge = 4
        };

        ViewSize m_viewSize;
        int      m_itemsPerRow;

        int  m_selectedItem;
        bool m_showContextMenu;

        bool m_showHiddenFiles;
        bool m_isOpen;

        static constexpr int         ITEM_SIZES[]      = { 64, 80, 96, 128, 160 };
        static constexpr const char *VIEW_SIZE_NAMES[] = { "Very Small", "Small", "Medium", "Large", "Very Large" };

    public:
        AssetBrowser( );
        ~AssetBrowser( );

        void Render( );
        void SetRootPath( const std::string &path );
        void NavigateToPath( const std::string &path );
        void RefreshCurrentDirectory( );

    private:
        void RenderToolbar( );
        void RenderNavigationBar( );
        void RenderAssetGrid( );
        void RenderAssetItem( const AssetItem &item, int index );
        void RenderContextMenu( );

        static const char *GetFileIcon( const AssetItem &item );
        static ImVec4      GetFileColor( const AssetItem &item );
        static std::string FormatFileSize( size_t bytes );

        void NavigateUp( );
        void NavigateBack( );
        void NavigateForward( );
        void GoHome( );

        void        ScanCurrentDirectory( );
        static bool IsValidPath( const std::string &path );
    };
} // namespace DZEngine
