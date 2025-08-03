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
#include "DZEngine/Rendering/GraphicsContext.h"
#include "MeshBatch.h"

namespace DZEngine
{
    /// This class is the main entry point to load assets; it is by design batched. Ideally, you want to push the batcher as much as possible.
    /// Implemented for GPUDriven code first, each batch will be a single draw call and will produce its own scene data.
    /// For large worlds you may want to create multiple batches, each resource type is batched into their pertinent batch class, for example, meshes are batched into MeshBatch
    /// Textures are batched into TextureBatch. These classes actually directly create their resources in the GPU.
    ///
    /// Batches are not loaded until you call `Load( batchId )`, this allows you to batch scenes before you need to load them, such as in large open world systems.
    ///
    /// WIP, currently only MeshBatch is implemented
    struct AssetBatcherDesc
    {
        GraphicsContext *GraphicsContext;
    };

    class AssetBatcher
    {
        struct AssetBatch
        {
            std::vector<std::function<void( )>> MeshRequests;
            std::unique_ptr<MeshBatch>          MeshBatch;
        };

        GraphicsContext        *m_graphicsContext;
        std::vector<AssetBatch> m_batches;

    public:
        explicit AssetBatcher( const AssetBatcherDesc &desc );
        ~AssetBatcher( );

        size_t NewBatch( );

        void              AddMesh( size_t batchId, BinaryReader& reader );
        void              AddGeometry( );
        std::future<bool> Load( size_t batchId );

        MeshBatch const *Mesh( size_t batchId );
    };
} // namespace DZEngine
