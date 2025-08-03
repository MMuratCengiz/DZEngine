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
    struct MeshPoolDesc
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

    class MeshPool
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

    public:
        explicit MeshPool( const MeshPoolDesc &desc );

        GPUMesh    AddMesh( BinaryReader &reader );
        GPUMesh    AddGeometry( const GeometryData *geometry, const Float4 &color = { 1.0f, 1.0f, 1.0f, 1.0f } );
        GPUSubMesh GetSubMesh( MeshHandle handle ) const;

        GPUBufferView GetVertexBuffer( ) const;
        GPUBufferView GetIndexBuffer( ) const;

    private:
        size_t NextHandle( );
    };
} // namespace DZEngine
