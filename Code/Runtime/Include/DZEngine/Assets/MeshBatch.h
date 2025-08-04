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

#include <DenOfIzGraphics/DenOfIzGraphics.h>
#include "DZEngine/Components/AssetHandle.h"
#include "DenOfIzGraphics/Support/GPUBufferView.h"
#include "MeshAssetData.h"

namespace DZEngine
{
    struct MeshBatchDesc
    {
        ILogicalDevice *LogicalDevice;

        size_t MaxVertexBufferBytes = 67108864;
        size_t MaxIndexBufferBytes  = 33554432;
    };

    struct GPUSubMesh
    {
        MeshHandle    Handle;
        SubMeshData  *Metadata;
        GPUBufferView VertexBuffer;
        GPUBufferView IndexBuffer;
    };

    struct GPUMesh
    {
        MeshAssetData          *Metadata;
        std::vector<GPUSubMesh> SubMeshes;
    };

    class MeshBatch
    {
        ILogicalDevice *m_logicalDevice;

        std::unique_ptr<IBufferResource> m_vertexBuffer;
        std::unique_ptr<IBufferResource> m_indexBuffer;

        size_t m_nextVertexOffset = 0;
        size_t m_nextIndexOffset  = 0;

        std::vector<std::unique_ptr<MeshAssetData>> m_meshDataStorage;

        std::mutex m_nextHandleLock;
        size_t     m_nextHandle = 0;

        std::mutex              m_newMeshLock;
        std::vector<GPUMesh>    m_meshes;
        std::vector<GPUSubMesh> m_subMeshes;

        std::unordered_map<std::string, MeshHandle> m_aliases;
        std::unordered_map<std::string, size_t>     m_parentMeshes;
        std::unique_ptr<BatchResourceCopy>          m_batchResourceCopy;

    public:
        explicit MeshBatch( const MeshBatchDesc &desc );

        void BeginUpdate( );
        void EndUpdate( ISemaphore *onComplete = nullptr ); // nullptr will block execution

        GPUMesh AddMesh( BinaryReader &reader, const std::vector<std::string> &aliases = { /*Index matches submesh index*/ } );
        GPUMesh AddGeometry( const GeometryData *geometry, std::string alias = "" );

        [[nodiscard]] GPUSubMesh    GetSubMesh( MeshHandle handle ) const;
        [[nodiscard]] GPUBufferView GetVertexBuffer( ) const;
        [[nodiscard]] GPUBufferView GetIndexBuffer( ) const;

        GPUMesh    GetParentMesh( const std::string &subMeshAlias );
        GPUSubMesh GetSubMesh( const std::string &alias ) const;

    private:
        size_t NextHandle( const std::string &alias );
    };
} // namespace DZEngine
