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
#include "AnimationBatch.h"
#include "AssetBundle.h"
#include "AssetRegistry.h"
#include "DZEngine/Rendering/GraphicsContext.h"
#include "MaterialBatch.h"
#include "MeshBatch.h"
#include "SkeletonBatch.h"

namespace DZEngine
{
    /// This class is the main entry point to load assets; it is by design batched. Ideally, you want to push the batcher as much as possible.
    /// Implemented for GPUDriven code first, each batch will be a single draw call and will produce its own scene data.
    /// For large worlds you may want to create multiple batches, each resource type is batched into their pertinent batch class, for example, meshes are batched into MeshBatch
    /// Textures are batched into TextureBatch. These classes actually directly create their resources in the GPU.
    ///
    /// Batches need to explicit
    ///
    /// WIP, currently only MeshBatch is implemented
    struct AssetBatcherDesc
    {
        GraphicsContext *GraphicsContext;
        AssetBundle     *AssetBundle;
        AssetRegistry   *AssetRegistry;
    };

    class AssetBatcher
    {
        struct AssetBatch
        {
            std::unique_ptr<MeshBatch>      MeshBatch;
            std::unique_ptr<MaterialBatch>  MaterialBatch;
            std::unique_ptr<AnimationBatch> AnimationBatch;
            std::unique_ptr<SkeletonBatch>  SkeletonBatch;
        };

        GraphicsContext *m_graphicsContext;
        AssetBundle     *m_assetBundle;
        AssetRegistry   *m_assetRegistry;

        std::mutex                               m_addBatchMutex;
        std::vector<std::unique_ptr<AssetBatch>> m_batches;
        std::unordered_map<std::string, size_t>  m_batchAliases;

    public:
        explicit AssetBatcher( const AssetBatcherDesc &desc );
        ~AssetBatcher( ) = default;

        size_t AddBatch( const std::string &alias, GeometryLayout layout = GeometryLayout::GPUDriven );
        size_t NumBatches( ) const;

        void BeginBatchUpdate( size_t batchId = 0 ) const;
        void EndBatchUpdate( size_t batchId = 0 ) const;

        void       AddMesh( BinaryReader &reader, const std::vector<std::string> &submeshAliases = { } ) const;
        void       AddGeometry( const GeometryData *data, const std::string &alias ) const;
        void       AddMesh( size_t batchId, BinaryReader &reader, const std::vector<std::string> &submeshAliases = { } ) const;
        MeshHandle AddMesh( size_t batchId, const std::string &uri, const std::vector<std::string> &submeshAliases = { } ) const;
        void       AddGeometry( size_t batchId, const GeometryData *data, const std::string &alias ) const;

        TextureHandle  LoadTexture( const std::string &alias, const std::string &uri ) const;
        TextureHandle  LoadTexture( size_t batchId, const std::string &alias, const std::string &uri ) const;
        TextureHandle  AddTexture( const std::string &alias, ITextureResource *texture ) const;
        TextureHandle  AddTexture( size_t batchId, const std::string &alias, ITextureResource *texture ) const;
        MaterialHandle AddMaterial( const std::string &alias, const MaterialDataRequest &material ) const;
        MaterialHandle AddMaterial( size_t batchId, const std::string &alias, const MaterialDataRequest &material ) const;

        AnimationClipHandle AddAnimation( size_t batchId, const std::string &uri, const std::string &alias ) const;
        AnimationClipHandle AddAnimation( const std::string &uri, const std::string &alias ) const;

        SkeletonHandle AddSkeleton( size_t batchId, const std::string &uri, const std::string &alias ) const;
        SkeletonHandle AddSkeleton( const std::string &uri, const std::string &alias ) const;

        MeshBatch const      *Mesh( size_t batchId = 0 ) const;
        MaterialBatch const  *Material( size_t batchId = 0 ) const;
        AnimationBatch const *Animation( size_t batchId = 0 ) const;
        SkeletonBatch const  *Skeleton( size_t batchId = 0 ) const;
    };
} // namespace DZEngine
