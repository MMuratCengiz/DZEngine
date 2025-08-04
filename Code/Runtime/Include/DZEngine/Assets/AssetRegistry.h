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
#include <string>
#include <unordered_map>
#include "DZEngine/Components/AssetHandle.h"

namespace DZEngine
{
    struct AssetEntry
    {
        std::string Uri;
        std::string AssetType; // "mesh", "material", "texture", etc.
    };

    /**
     * AssetRegistry manages the mapping between BatchId + Handle pairs and asset URIs.
     * This allows components to store lightweight BatchId/Handle references while
     * maintaining the connection to actual asset files.
     *
     * Registry format:
     * {
     *   "batches": {
     *     "0": {
     *       "meshes": {
     *         "1": { "uri": "assets://models/cube.obj", "type": "mesh" },
     *         "2": { "uri": "assets://models/sphere.obj", "type": "mesh" }
     *       }
     *     }
     *   }
     * }
     */
    class AssetRegistry
    {
        // BatchId -> HandleId -> AssetEntry
        std::unordered_map<size_t, std::unordered_map<uint32_t, AssetEntry>> m_meshRegistry;
        std::unordered_map<size_t, std::unordered_map<uint32_t, AssetEntry>> m_materialRegistry;
        std::unordered_map<size_t, std::unordered_map<uint32_t, AssetEntry>> m_textureRegistry;

        std::filesystem::path m_registryPath;

    public:
        AssetRegistry() = default;
        explicit AssetRegistry( const std::filesystem::path &registryPath );

        bool LoadFromFile( const std::filesystem::path &registryPath );
        bool SaveToFile( const std::filesystem::path &registryPath );

        void RegisterMeshAsset( size_t batchId, MeshHandle handle, const std::string &uri );
        void RegisterMaterialAsset( size_t batchId, MaterialHandle handle, const std::string &uri );
        void RegisterTextureAsset( size_t batchId, TextureHandle handle, const std::string &uri );

        bool GetMeshAssetUri( size_t batchId, MeshHandle handle, std::string &outUri );
        bool GetMaterialAssetUri( size_t batchId, MaterialHandle handle, std::string &outUri );
        bool GetTextureAssetUri( size_t batchId, TextureHandle handle, std::string &outUri );

        void ClearBatch( size_t batchId );
        void Clear( );

        void LogRegistry( );

        const std::filesystem::path &GetRegistryPath( ) const { return m_registryPath; }
        void SetRegistryPath( const std::filesystem::path &path ) { m_registryPath = path; }
    };
} // namespace DZEngine
