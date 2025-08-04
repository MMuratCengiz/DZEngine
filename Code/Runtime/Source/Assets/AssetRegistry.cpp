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

#include "DZEngine/Assets/AssetRegistry.h"
#include <fstream>
#include <nlohmann/json.hpp>
#include <set>
#include <spdlog/spdlog.h>

using namespace DZEngine;
using json = nlohmann::json;

AssetRegistry::AssetRegistry( const std::filesystem::path &registryPath ) : m_registryPath( registryPath )
{
    spdlog::info( "AssetRegistry: Created with registry path: {}", registryPath.string( ) );
}

bool AssetRegistry::LoadFromFile( const std::filesystem::path &registryPath )
{
    m_registryPath = registryPath;

    if ( !std::filesystem::exists( registryPath ) )
    {
        spdlog::info( "AssetRegistry::LoadFromFile - Registry file not found, starting with empty registry: {}", registryPath.string( ) );
        return true;
    }

    std::ifstream file( registryPath );
    if ( !file.is_open( ) )
    {
        spdlog::error( "AssetRegistry::LoadFromFile - Failed to open registry file: {}", registryPath.string( ) );
        return false;
    }

    std::string jsonData( ( std::istreambuf_iterator<char>( file ) ), std::istreambuf_iterator<char>( ) );
    file.close( );

    if ( !json::accept( jsonData ) )
    {
        spdlog::error( "AssetRegistry::LoadFromFile - Invalid JSON in registry file: {}", registryPath.string( ) );
        return false;
    }

    json registryJson = json::parse( jsonData );

    // Clear existing registries
    Clear( );

    // Load batches
    if ( registryJson.contains( "batches" ) && registryJson[ "batches" ].is_object( ) )
    {
        for ( const auto &[ batchIdStr, batchData ] : registryJson[ "batches" ].items( ) )
        {
            size_t batchId = std::stoull( batchIdStr );

            // Load meshes
            if ( batchData.contains( "meshes" ) && batchData[ "meshes" ].is_object( ) )
            {
                for ( const auto &[ handleIdStr, assetData ] : batchData[ "meshes" ].items( ) )
                {
                    uint32_t   handleId = std::stoul( handleIdStr );
                    AssetEntry entry;
                    entry.Uri                             = assetData.value( "uri", "" );
                    entry.AssetType                       = assetData.value( "type", "mesh" );
                    m_meshRegistry[ batchId ][ handleId ] = entry;
                }
            }

            // Load materials
            if ( batchData.contains( "materials" ) && batchData[ "materials" ].is_object( ) )
            {
                for ( const auto &[ handleIdStr, assetData ] : batchData[ "materials" ].items( ) )
                {
                    uint32_t   handleId = std::stoul( handleIdStr );
                    AssetEntry entry;
                    entry.Uri                                 = assetData.value( "uri", "" );
                    entry.AssetType                           = assetData.value( "type", "material" );
                    m_materialRegistry[ batchId ][ handleId ] = entry;
                }
            }

            // Load textures
            if ( batchData.contains( "textures" ) && batchData[ "textures" ].is_object( ) )
            {
                for ( const auto &[ handleIdStr, assetData ] : batchData[ "textures" ].items( ) )
                {
                    uint32_t   handleId = std::stoul( handleIdStr );
                    AssetEntry entry;
                    entry.Uri                                = assetData.value( "uri", "" );
                    entry.AssetType                          = assetData.value( "type", "texture" );
                    m_textureRegistry[ batchId ][ handleId ] = entry;
                }
            }

            if ( batchData.contains( "animations" ) && batchData[ "animations" ].is_object( ) )
            {
                for ( const auto &[ handleIdStr, assetData ] : batchData[ "animations" ].items( ) )
                {
                    uint32_t   handleId = std::stoul( handleIdStr );
                    AssetEntry entry;
                    entry.Uri                                  = assetData.value( "uri", "" );
                    entry.AssetType                            = assetData.value( "type", "animation" );
                    m_animationRegistry[ batchId ][ handleId ] = entry;
                }
            }

            if ( batchData.contains( "skeletons" ) && batchData[ "skeletons" ].is_object( ) )
            {
                for ( const auto &[ handleIdStr, assetData ] : batchData[ "skeletons" ].items( ) )
                {
                    uint32_t   handleId = std::stoul( handleIdStr );
                    AssetEntry entry;
                    entry.Uri                                 = assetData.value( "uri", "" );
                    entry.AssetType                           = assetData.value( "type", "skeleton" );
                    m_skeletonRegistry[ batchId ][ handleId ] = entry;
                }
            }
        }
    }

    spdlog::info( "AssetRegistry::LoadFromFile - Loaded asset registry from: {}", registryPath.string( ) );
    return true;
}

bool AssetRegistry::SaveToFile( const std::filesystem::path &registryPath )
{
    m_registryPath = registryPath;

    json registryJson;
    registryJson[ "batches" ] = json::object( );
    std::set<size_t> allBatchIds;
    for ( const auto &batchId : m_meshRegistry | std::views::keys )
    {
        allBatchIds.insert( batchId );
    }
    for ( const auto &batchId : m_materialRegistry | std::views::keys )
    {
        allBatchIds.insert( batchId );
    }
    for ( const auto &batchId : m_textureRegistry | std::views::keys )
    {
        allBatchIds.insert( batchId );
    }
    for ( const auto &batchId : m_animationRegistry | std::views::keys )
    {
        allBatchIds.insert( batchId );
    }
    for ( const auto &batchId : m_skeletonRegistry | std::views::keys )
    {
        allBatchIds.insert( batchId );
    }
    for ( size_t batchId : allBatchIds )
    {
        std::string batchIdStr                  = std::to_string( batchId );
        registryJson[ "batches" ][ batchIdStr ] = json::object( );
        if ( m_meshRegistry.contains( batchId ) )
        {
            registryJson[ "batches" ][ batchIdStr ][ "meshes" ] = json::object( );
            for ( const auto &[ handleId, entry ] : m_meshRegistry[ batchId ] )
            {
                std::string handleIdStr                                            = std::to_string( handleId );
                registryJson[ "batches" ][ batchIdStr ][ "meshes" ][ handleIdStr ] = { { "uri", entry.Uri }, { "type", entry.AssetType } };
            }
        }
        if ( m_materialRegistry.contains( batchId ) )
        {
            registryJson[ "batches" ][ batchIdStr ][ "materials" ] = json::object( );
            for ( const auto &[ handleId, entry ] : m_materialRegistry[ batchId ] )
            {
                std::string handleIdStr                                               = std::to_string( handleId );
                registryJson[ "batches" ][ batchIdStr ][ "materials" ][ handleIdStr ] = { { "uri", entry.Uri }, { "type", entry.AssetType } };
            }
        }
        if ( m_textureRegistry.contains( batchId ) )
        {
            registryJson[ "batches" ][ batchIdStr ][ "textures" ] = json::object( );
            for ( const auto &[ handleId, entry ] : m_textureRegistry[ batchId ] )
            {
                std::string handleIdStr                                              = std::to_string( handleId );
                registryJson[ "batches" ][ batchIdStr ][ "textures" ][ handleIdStr ] = { { "uri", entry.Uri }, { "type", entry.AssetType } };
            }
        }
        if ( m_animationRegistry.contains( batchId ) )
        {
            registryJson[ "batches" ][ batchIdStr ][ "animations" ] = json::object( );
            for ( const auto &[ handleId, entry ] : m_animationRegistry[ batchId ] )
            {
                std::string handleIdStr                                                = std::to_string( handleId );
                registryJson[ "batches" ][ batchIdStr ][ "animations" ][ handleIdStr ] = { { "uri", entry.Uri }, { "type", entry.AssetType } };
            }
        }
        if ( m_skeletonRegistry.contains( batchId ) )
        {
            registryJson[ "batches" ][ batchIdStr ][ "skeletons" ] = json::object( );
            for ( const auto &[ handleId, entry ] : m_skeletonRegistry[ batchId ] )
            {
                std::string handleIdStr                                               = std::to_string( handleId );
                registryJson[ "batches" ][ batchIdStr ][ "skeletons" ][ handleIdStr ] = { { "uri", entry.Uri }, { "type", entry.AssetType } };
            }
        }
    }

    std::error_code ec;
    std::filesystem::create_directories( registryPath.parent_path( ), ec );
    if ( ec )
    {
        spdlog::error( "AssetRegistry::SaveToFile - Failed to create directories: {}", ec.message( ) );
        return false;
    }

    std::ofstream file( registryPath );
    if ( !file.is_open( ) )
    {
        spdlog::error( "AssetRegistry::SaveToFile - Failed to create registry file: {}", registryPath.string( ) );
        return false;
    }

    file << registryJson.dump( 2 );
    file.close( );

    if ( file.fail( ) )
    {
        spdlog::error( "AssetRegistry::SaveToFile - Failed to write registry file: {}", registryPath.string( ) );
        return false;
    }

    spdlog::info( "AssetRegistry::SaveToFile - Saved asset registry to: {}", registryPath.string( ) );
    return true;
}

void AssetRegistry::RegisterMeshAsset( size_t batchId, MeshHandle handle, const std::string &uri )
{
    AssetEntry entry;
    entry.Uri                              = uri;
    entry.AssetType                        = "mesh";
    m_meshRegistry[ batchId ][ handle.Id ] = entry;

    spdlog::debug( "AssetRegistry::RegisterMeshAsset - Batch: {}, Handle: {}, URI: {}", batchId, handle.Id, uri );
}

void AssetRegistry::RegisterMaterialAsset( size_t batchId, MaterialHandle handle, const std::string &uri )
{
    AssetEntry entry;
    entry.Uri                                  = uri;
    entry.AssetType                            = "material";
    m_materialRegistry[ batchId ][ handle.Id ] = entry;

    spdlog::debug( "AssetRegistry::RegisterMaterialAsset - Batch: {}, Handle: {}, URI: {}", batchId, handle.Id, uri );
}

void AssetRegistry::RegisterTextureAsset( size_t batchId, TextureHandle handle, const std::string &uri )
{
    AssetEntry entry;
    entry.Uri                                 = uri;
    entry.AssetType                           = "texture";
    m_textureRegistry[ batchId ][ handle.Id ] = entry;

    spdlog::debug( "AssetRegistry::RegisterTextureAsset - Batch: {}, Handle: {}, URI: {}", batchId, handle.Id, uri );
}

void AssetRegistry::RegisterAnimationAsset( size_t batchId, AnimationClipHandle handle, const std::string &uri )
{
    AssetEntry entry;
    entry.Uri                                   = uri;
    entry.AssetType                             = "animation";
    m_animationRegistry[ batchId ][ handle.Id ] = entry;

    spdlog::debug( "AssetRegistry::RegisterAnimationAsset - Batch: {}, Handle: {}, URI: {}", batchId, handle.Id, uri );
}

void AssetRegistry::RegisterSkeletonAsset( size_t batchId, SkeletonHandle handle, const std::string &uri )
{
    AssetEntry entry;
    entry.Uri                                  = uri;
    entry.AssetType                            = "skeleton";
    m_skeletonRegistry[ batchId ][ handle.Id ] = entry;

    spdlog::debug( "AssetRegistry::RegisterSkeletonAsset - Batch: {}, Handle: {}, URI: {}", batchId, handle.Id, uri );
}

bool AssetRegistry::GetMeshAssetUri( const size_t batchId, const MeshHandle handle, std::string &outUri )
{
    if ( m_meshRegistry.contains( batchId ) && m_meshRegistry[ batchId ].contains( handle.Id ) )
    {
        outUri = m_meshRegistry[ batchId ][ handle.Id ].Uri;
        return true;
    }
    return false;
}

bool AssetRegistry::GetMaterialAssetUri( const size_t batchId, const MaterialHandle handle, std::string &outUri )
{
    if ( m_materialRegistry.contains( batchId ) && m_materialRegistry[ batchId ].contains( handle.Id ) )
    {
        outUri = m_materialRegistry[ batchId ][ handle.Id ].Uri;
        return true;
    }
    return false;
}

bool AssetRegistry::GetTextureAssetUri( const size_t batchId, const TextureHandle handle, std::string &outUri )
{
    if ( m_textureRegistry.contains( batchId ) && m_textureRegistry[ batchId ].contains( handle.Id ) )
    {
        outUri = m_textureRegistry[ batchId ][ handle.Id ].Uri;
        return true;
    }
    return false;
}

bool AssetRegistry::GetAnimationAssetUri( const size_t batchId, const AnimationClipHandle handle, std::string &outUri )
{
    if ( m_animationRegistry.contains( batchId ) && m_animationRegistry[ batchId ].contains( handle.Id ) )
    {
        outUri = m_animationRegistry[ batchId ][ handle.Id ].Uri;
        return true;
    }
    return false;
}

bool AssetRegistry::GetSkeletonAssetUri( const size_t batchId, const SkeletonHandle handle, std::string &outUri )
{
    if ( m_skeletonRegistry.contains( batchId ) && m_skeletonRegistry[ batchId ].contains( handle.Id ) )
    {
        outUri = m_skeletonRegistry[ batchId ][ handle.Id ].Uri;
        return true;
    }
    return false;
}

void AssetRegistry::ClearBatch( size_t batchId )
{
    m_meshRegistry.erase( batchId );
    m_materialRegistry.erase( batchId );
    m_textureRegistry.erase( batchId );
    m_animationRegistry.erase( batchId );
    m_skeletonRegistry.erase( batchId );

    spdlog::debug( "AssetRegistry::ClearBatch - Cleared batch: {}", batchId );
}

void AssetRegistry::Clear( )
{
    m_meshRegistry.clear( );
    m_materialRegistry.clear( );
    m_textureRegistry.clear( );
    m_animationRegistry.clear( );
    m_skeletonRegistry.clear( );

    spdlog::debug( "AssetRegistry::Clear - Cleared all registries" );
}

void AssetRegistry::LogRegistry( )
{
    spdlog::info( "=== AssetRegistry Contents ===" );

    std::set<size_t> allBatchIds;
    for ( const auto &batchId : m_meshRegistry | std::views::keys )
    {
        allBatchIds.insert( batchId );
    }
    for ( const auto &batchId : m_materialRegistry | std::views::keys )
    {
        allBatchIds.insert( batchId );
    }
    for ( const auto &batchId : m_textureRegistry | std::views::keys )
    {
        allBatchIds.insert( batchId );
    }
    for ( const auto &batchId : m_animationRegistry | std::views::keys )
    {
        allBatchIds.insert( batchId );
    }
    for ( const auto &batchId : m_skeletonRegistry | std::views::keys )
    {
        allBatchIds.insert( batchId );
    }

    for ( size_t batchId : allBatchIds )
    {
        spdlog::info( "Batch {}:", batchId );

        if ( m_meshRegistry.contains( batchId ) )
        {
            for ( const auto &[ handleId, entry ] : m_meshRegistry[ batchId ] )
            {
                spdlog::info( "  Mesh {}: {}", handleId, entry.Uri );
            }
        }

        if ( m_materialRegistry.contains( batchId ) )
        {
            for ( const auto &[ handleId, entry ] : m_materialRegistry[ batchId ] )
            {
                spdlog::info( "  Material {}: {}", handleId, entry.Uri );
            }
        }

        if ( m_textureRegistry.contains( batchId ) )
        {
            for ( const auto &[ handleId, entry ] : m_textureRegistry[ batchId ] )
            {
                spdlog::info( "  Texture {}: {}", handleId, entry.Uri );
            }
        }

        if ( m_animationRegistry.contains( batchId ) )
        {
            for ( const auto &[ handleId, entry ] : m_animationRegistry[ batchId ] )
            {
                spdlog::info( "  Animation {}: {}", handleId, entry.Uri );
            }
        }

        if ( m_skeletonRegistry.contains( batchId ) )
        {
            for ( const auto &[ handleId, entry ] : m_skeletonRegistry[ batchId ] )
            {
                spdlog::info( "  Skeleton {}: {}", handleId, entry.Uri );
            }
        }
    }
}

const std::filesystem::path &AssetRegistry::GetRegistryPath( ) const
{
    return m_registryPath;
}

void AssetRegistry::SetRegistryPath( const std::filesystem::path &path )
{
    m_registryPath = path;
}
