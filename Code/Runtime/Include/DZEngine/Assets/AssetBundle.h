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

#include <DenOfIzGraphics/Assets/Stream/BinaryReader.h>
#include <filesystem>
#include <memory>
#include <string>

using namespace DenOfIz;

namespace DZEngine
{
    enum class AssetLoadResult
    {
        Success,
        InvalidUri,
        FileNotFound,
        BundleNotFound,
        ReadError
    };

    /**
     * AssetBundle handles URI-based asset loading with support for:
     * - assets://relative/path/to/asset.ext (loads from assets directory)
     * - assets://bundle/path/to/asset.ext (loads from bundle file - TODO)
     *
     * Currently only implements file-based loading. Bundle support is reserved for future implementation.
     */
    class AssetBundle
    {
        std::filesystem::path m_assetsDirectory;

    public:
        explicit AssetBundle( const std::filesystem::path &assetsDir );

        std::unique_ptr<BinaryReader> LoadAsset( const std::string &uri ) const;
        bool                          AssetExists( const std::string &uri ) const;
        AssetLoadResult               ResolveUri( const std::string &uri, std::filesystem::path &outPath ) const;

        const std::filesystem::path &GetAssetsDirectory( ) const;

    private:
        bool        IsValidAssetUri( const std::string &uri ) const;
        std::string ExtractPathFromUri( const std::string &uri ) const;
    };
} // namespace DZEngine
