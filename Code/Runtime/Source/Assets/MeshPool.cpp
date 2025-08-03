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

#include "DZEngine/Assets/MeshPool.h"

#include <glm/common.hpp>
#include <glm/vec3.hpp>

#include "DZEngine/Assets/StaticMeshVertex.h"

#include <spdlog/spdlog.h>

using namespace DZEngine;

MeshPool::MeshPool( const MeshPoolDesc &desc ) : m_logicalDevice( desc.LogicalDevice )
{
    if ( !m_logicalDevice )
    {
        spdlog::error( "MeshPool: LogicalDevice is required" );
        return;
    }

    BufferDesc vertexBufferDesc{ };
    vertexBufferDesc.Descriptor = ResourceDescriptor::VertexBuffer;
    vertexBufferDesc.NumBytes   = desc.MaxVertexBufferBytes;
    vertexBufferDesc.HeapType   = HeapType::GPU;
    vertexBufferDesc.Usages     = ResourceUsage::CopyDst;
    vertexBufferDesc.DebugName  = "Mesh Pool Vertex Buffer";
    m_vertexBuffer              = std::unique_ptr<IBufferResource>( m_logicalDevice->CreateBufferResource( vertexBufferDesc ) );

    BufferDesc indexBufferDesc{ };
    indexBufferDesc.Descriptor = ResourceDescriptor::IndexBuffer;
    indexBufferDesc.NumBytes   = desc.MaxIndexBufferBytes;
    indexBufferDesc.HeapType   = HeapType::GPU;
    indexBufferDesc.Usages     = ResourceUsage::CopyDst;
    indexBufferDesc.DebugName  = "Mesh Pool Index Buffer";
    m_indexBuffer              = std::unique_ptr<IBufferResource>( m_logicalDevice->CreateBufferResource( indexBufferDesc ) );
}

void MeshPool::AddMesh( MeshHandle handle, BinaryReader &reader )
{
    MeshAssetReaderDesc meshReaderDesc{ };
    meshReaderDesc.Reader = &reader;
    MeshAssetReader            meshAssetReader( meshReaderDesc );
    std::unique_ptr<MeshAsset> meshAsset( meshAssetReader.Read( ) );

    if ( m_entries.size( ) <= handle.Id )
    {
        m_entries.resize( handle.Id + 1 );
    }

    auto  &meshAssetData = m_meshDataStorage.emplace_back( std::make_unique<MeshAssetData>( MeshAssetData::LoadFromMeshAsset( *meshAsset ) ) );
    size_t vertexOffset  = m_nextVertexOffset;
    size_t indexOffset   = m_nextIndexOffset;
    {
        std::lock_guard lock( m_newMeshLock );
        m_nextVertexOffset = m_nextVertexOffset + meshAssetData->GetTotalNumVertices( ) * meshAssetData->GetVertexNumBytes( );
        m_nextIndexOffset  = m_nextIndexOffset + meshAssetData->GetTotalNumIndices( ) * sizeof( uint32_t );
    }

    GPUMesh &newGPUMesh = m_entries[ handle.Id ];
    newGPUMesh.Metadata = meshAssetData.get( );

    BatchResourceCopy batchCopy( m_logicalDevice );
    batchCopy.Begin( );

    for ( uint32_t meshIndex = 0; meshIndex < meshAsset->SubMeshes.NumElements; ++meshIndex )
    {
        auto &gpuSubMesh    = newGPUMesh.SubMeshes.emplace_back( );
        gpuSubMesh.Metadata = &meshAssetData->SubMeshes[ meshIndex ];

        auto       &subMesh     = meshAsset->SubMeshes.Elements[ meshIndex ];
        std::string subMeshName = subMesh.Name.Get( );

        LoadAssetStreamToBufferDesc vertexLoadDesc;
        vertexLoadDesc.Stream          = subMesh.VertexStream;
        vertexLoadDesc.DstBuffer       = m_vertexBuffer.get( );
        vertexLoadDesc.DstBufferOffset = vertexOffset;
        vertexLoadDesc.Reader          = &reader;
        batchCopy.LoadAssetStreamToBuffer( vertexLoadDesc );

        gpuSubMesh.VertexBuffer.Buffer   = m_vertexBuffer.get( );
        gpuSubMesh.VertexBuffer.Offset   = vertexOffset;
        gpuSubMesh.VertexBuffer.NumBytes = subMesh.VertexStream.NumBytes;

        if ( subMesh.IndexStream.NumBytes > 0 )
        {
            LoadAssetStreamToBufferDesc indexLoadDesc;
            indexLoadDesc.Stream          = subMesh.IndexStream;
            indexLoadDesc.DstBuffer       = m_indexBuffer.get( );
            indexLoadDesc.DstBufferOffset = indexOffset;
            indexLoadDesc.Reader          = &reader;
            batchCopy.LoadAssetStreamToBuffer( indexLoadDesc );

            gpuSubMesh.IndexBuffer.Buffer   = m_indexBuffer.get( );
            gpuSubMesh.IndexBuffer.Offset   = indexOffset;
            gpuSubMesh.IndexBuffer.NumBytes = subMesh.IndexStream.NumBytes;
        }
    }

    batchCopy.Submit( );
}

void MeshPool::AddGeometry( MeshHandle handle, const GeometryData *geometry, const Float4 &color )
{
    if ( !geometry )
    {
        spdlog::error( "AddGeometry: geometry is required" );
        return;
    }

    if ( m_entries.size( ) <= handle.Id )
    {
        m_entries.resize( handle.Id + 1 );
    }
    GPUMesh &newGPUMesh = m_entries[ handle.Id ];

    const auto &geometryData = *geometry;

    const uint32_t numVertices = geometryData.Vertices.NumElements;
    const uint32_t numIndices  = geometryData.Indices.NumElements;

    if ( numVertices == 0 )
    {
        spdlog::error( "LoadGeometry: No vertices in geometry data" );
        return;
    }

    std::vector<StaticMeshVertex> vertices;
    vertices.reserve( numVertices );

    auto minBounds = glm::vec3( std::numeric_limits<float>::max( ) );
    auto maxBounds = glm::vec3( std::numeric_limits<float>::lowest( ) );

    for ( uint32_t i = 0; i < numVertices; ++i )
    {
        const auto      &geoVertex = geometryData.Vertices.Elements[ i ];
        StaticMeshVertex vertex{ };

        vertex.Position.X = geoVertex.Position.X;
        vertex.Position.Y = geoVertex.Position.Y;
        vertex.Position.Z = geoVertex.Position.Z;
        vertex.Position.W = 1.0f;

        vertex.Normal.X = geoVertex.Normal.X;
        vertex.Normal.Y = geoVertex.Normal.Y;
        vertex.Normal.Z = geoVertex.Normal.Z;
        vertex.Normal.W = 0.0f;

        vertex.TexCoord.X = geoVertex.TextureCoordinate.U;
        vertex.TexCoord.Y = geoVertex.TextureCoordinate.V;

        vertex.Color = { color.X, color.Y, color.Z, color.W };

        vertex.Tangent = { 0.0f, 0.0f, 0.0f, 0.0f };

        vertices.push_back( vertex );

        glm::vec3 pos( vertex.Position.X, vertex.Position.Y, vertex.Position.Z );

        minBounds = glm::min( minBounds, pos );
        maxBounds = glm::max( maxBounds, pos );
    }

    const size_t numVertexBytes = vertices.size( ) * sizeof( StaticMeshVertex );
    const size_t numIndexBytes  = numIndices * sizeof( uint32_t );

    size_t vertexOffset = m_nextVertexOffset;
    size_t indexOffset  = m_nextIndexOffset;
    {
        std::lock_guard lock( m_newMeshLock );
        m_nextVertexOffset = m_nextVertexOffset + numVertexBytes;
        m_nextIndexOffset  = m_nextIndexOffset + numIndexBytes;
    }

    BatchResourceCopy batchCopy( m_logicalDevice );
    batchCopy.Begin( );

    CopyToGpuBufferDesc vertexCopyDesc{ };
    vertexCopyDesc.DstBuffer       = m_vertexBuffer.get( );
    vertexCopyDesc.DstBufferOffset = vertexOffset;
    vertexCopyDesc.Data            = { reinterpret_cast<const Byte *>( vertices.data( ) ), numVertexBytes };
    batchCopy.CopyToGPUBuffer( vertexCopyDesc );

    if ( numIndices > 0 )
    {
        CopyToGpuBufferDesc indexCopyDesc{ };
        indexCopyDesc.DstBuffer       = m_indexBuffer.get( );
        indexCopyDesc.DstBufferOffset = indexOffset;
        indexCopyDesc.Data            = { reinterpret_cast<const Byte *>( geometryData.Indices.Elements ), numIndexBytes };
        batchCopy.CopyToGPUBuffer( indexCopyDesc );
    }

    batchCopy.Submit( );

    auto &meshAssetData                                 = m_meshDataStorage.emplace_back( std::make_unique<MeshAssetData>( MeshAssetData{ } ) );
    newGPUMesh.Metadata                                 = meshAssetData.get( );
    newGPUMesh.Metadata->Name                           = "MeshPool Geometry";
    newGPUMesh.Metadata->EnabledAttributes.Position     = true;
    newGPUMesh.Metadata->EnabledAttributes.Normal       = true;
    newGPUMesh.Metadata->EnabledAttributes.UV           = true;
    newGPUMesh.Metadata->EnabledAttributes.Color        = true;
    newGPUMesh.Metadata->EnabledAttributes.Tangent      = true;
    newGPUMesh.Metadata->EnabledAttributes.Bitangent    = false;
    newGPUMesh.Metadata->EnabledAttributes.BlendIndices = false;
    newGPUMesh.Metadata->EnabledAttributes.BlendWeights = false;

    GPUSubMesh &subMesh           = newGPUMesh.SubMeshes.emplace_back( );
    subMesh.VertexBuffer.Buffer   = m_vertexBuffer.get( );
    subMesh.VertexBuffer.Offset   = vertexOffset;
    subMesh.VertexBuffer.NumBytes = numVertexBytes;
    subMesh.IndexBuffer.Buffer    = m_indexBuffer.get( );
    subMesh.IndexBuffer.Offset    = indexOffset;
    subMesh.IndexBuffer.NumBytes  = numIndexBytes;

    subMesh.Metadata              = &meshAssetData->SubMeshes.emplace_back( );
    subMesh.Metadata->NumVertices = numVertices;
    subMesh.Metadata->NumIndices  = numIndices;
    subMesh.Metadata->IndexType   = IndexType::Uint32;
    subMesh.Metadata->MinBounds   = { minBounds.x, minBounds.y, minBounds.z };
    subMesh.Metadata->MaxBounds   = { maxBounds.x, maxBounds.y, maxBounds.z };
}

GPUBufferView MeshPool::GetVertexBuffer( ) const
{
    return GPUBufferView{ .Buffer = m_vertexBuffer.get( ), .NumBytes = m_nextVertexOffset, .Offset = 0 };
}

GPUBufferView MeshPool::GetIndexBuffer( ) const
{
    return GPUBufferView{ .Buffer = m_indexBuffer.get( ), .NumBytes = m_nextIndexOffset, .Offset = 0 };
}
