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

#include "DZEngine/Assets/AssetBundle.h"
#include <spdlog/spdlog.h>

using namespace DZEngine;

AssetBundle::AssetBundle( const std::filesystem::path &assetsDir ) : m_assetsDirectory( assetsDir )
{
    spdlog::info( "AssetBundle: Created with assets directory: {}", assetsDir.string( ) );
}

std::unique_ptr<BinaryReader> AssetBundle::LoadAsset( const std::string &uri ) const
{
    if ( !IsValidAssetUri( uri ) )
    {
        spdlog::error( "AssetBundle::LoadAsset - Invalid asset URI: {}", uri );
        return nullptr;
    }

    std::filesystem::path resolvedPath;
    if ( const AssetLoadResult result = ResolveUri( uri, resolvedPath ); result != AssetLoadResult::Success )
    {
        spdlog::error( "AssetBundle::LoadAsset - Failed to resolve URI: {}", uri );
        return nullptr;
    }

    if ( !std::filesystem::exists( resolvedPath ) )
    {
        spdlog::error( "AssetBundle::LoadAsset - File not found: {}", resolvedPath.string( ) );
        return nullptr;
    }

    auto reader = std::make_unique<BinaryReader>( resolvedPath.string( ).c_str( ) );
    return reader;
}

bool AssetBundle::AssetExists( const std::string &uri ) const
{
    if ( !IsValidAssetUri( uri ) )
    {
        return false;
    }

    std::filesystem::path resolvedPath;
    if ( ResolveUri( uri, resolvedPath ) != AssetLoadResult::Success )
    {
        return false;
    }

    return std::filesystem::exists( resolvedPath );
}

AssetLoadResult AssetBundle::ResolveUri( const std::string &uri, std::filesystem::path &outPath ) const
{
    if ( !IsValidAssetUri( uri ) )
    {
        return AssetLoadResult::InvalidUri;
    }

    const std::string relativePath = ExtractPathFromUri( uri );

    // TODO no bundling atm, always used as relative paths
    outPath = m_assetsDirectory / relativePath;
    return AssetLoadResult::Success;
}

const std::filesystem::path &AssetBundle::GetAssetsDirectory( ) const
{
    return m_assetsDirectory;
}

bool AssetBundle::IsValidAssetUri( const std::string &uri ) const
{
    const std::string prefix = "assets://";
    if ( uri.length( ) <= prefix.length( ) || uri.substr( 0, prefix.length( ) ) != prefix )
    {
        return false;
    }
    if ( const std::string path = uri.substr( prefix.length( ) ); path.empty( ) || path[ 0 ] == '/' )
    {
        return false;
    }
    return true;
}

std::string AssetBundle::ExtractPathFromUri( const std::string &uri ) const
{
    const std::string prefix = "assets://";
    if ( uri.length( ) <= prefix.length( ) )
    {
        return "";
    }

    return uri.substr( prefix.length( ) );
}
