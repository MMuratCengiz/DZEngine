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

#include "DZEngine/Rendering/GPUDriven/GPUDrivenDataUpload.h"

#include <cmath>
#include <flecs.h>
#include "DZEngine/Assets/StaticMeshVertex.h"
#include "DZEngine/Components/CameraComponent.h"
#include "DZEngine/Components/Graphics/MaterialComponent.h"
#include "DZEngine/Components/Graphics/MeshComponent.h"
#include "DZEngine/Components/Graphics/RenderableComponent.h"
#include "DZEngine/Components/TransformComponent.h"
#include "DZEngine/Utilities/DataUtilities.h"

#include "DZEngine/Math/MathConverter.h"
#include "DZEngine/Scene/World.h"

using namespace DZEngine;
using namespace DenOfIz;

GPUDrivenDataUpload::GPUDrivenDataUpload( const GPUDrivenDataUploadDesc &uploadDesc ) :
    m_logicalDevice( uploadDesc.GraphicsContext->LogicalDevice ), m_world( uploadDesc.World ), m_assets( uploadDesc.Assets ), m_uploadDesc( uploadDesc )
{
    m_batchId   = uploadDesc.BatchId;
    m_copyQueue = std::unique_ptr<ICommandQueue>( m_logicalDevice->CreateCommandQueue( CommandQueueDesc{ QueueType::Copy } ) );

    m_frames.resize( uploadDesc.NumFrames );
    for ( size_t i = 0; i < m_frames.size( ); ++i )
    {
        m_frames[ i ] = std::make_unique<FrameData>( );

        CommandListPoolDesc poolDesc{ };
        poolDesc.CommandQueue    = m_copyQueue.get( );
        poolDesc.NumCommandLists = 6;

        m_frames[ i ]->OnComplete      = std::unique_ptr<ISemaphore>( m_logicalDevice->CreateSemaphore( ) );
        m_frames[ i ]->CommandListPool = std::unique_ptr<ICommandListPool>( m_logicalDevice->CreateCommandListPool( poolDesc ) );
        m_frames[ i ]->CommandLists    = m_frames[ i ]->CommandListPool->GetCommandLists( );
    }

    const size_t maxNumObjects        = uploadDesc.MaxObjects;
    m_dataRanges.ObjectBufferOffset   = 0;
    m_dataRanges.ObjectBufferNumBytes = sizeof( GPUObjectData ) * maxNumObjects;

    const size_t maxNumMaterials        = uploadDesc.MaxMaterials;
    m_dataRanges.MaterialBufferOffset   = DataUtilities::Align( m_dataRanges.ObjectBufferNumBytes, 256 );
    m_dataRanges.MaterialBufferNumBytes = sizeof( GPUMaterialData ) * maxNumMaterials;

    const size_t maxNumMeshes       = uploadDesc.MaxMeshes;
    m_dataRanges.MeshBufferOffset   = DataUtilities::Align( m_dataRanges.MaterialBufferNumBytes + m_dataRanges.MaterialBufferOffset, 256 );
    m_dataRanges.MeshBufferNumBytes = sizeof( GPUMeshData ) * maxNumMeshes;

    const size_t maxNumInstances        = maxNumObjects;
    m_dataRanges.InstanceBufferOffset   = DataUtilities::Align( m_dataRanges.MeshBufferNumBytes + m_dataRanges.MeshBufferOffset, 256 );
    m_dataRanges.InstanceBufferNumBytes = sizeof( GPUInstanceData ) * maxNumInstances;

    const size_t maxDrawArgs            = maxNumObjects;
    m_dataRanges.DrawArgsBufferOffset   = DataUtilities::Align( m_dataRanges.InstanceBufferNumBytes + m_dataRanges.InstanceBufferOffset, 256 );
    m_dataRanges.DrawArgsBufferNumBytes = sizeof( DrawArguments ) * maxDrawArgs;

    const size_t maxIndirectCommands    = maxNumObjects;
    m_dataRanges.IndirectBufferOffset   = DataUtilities::Align( m_dataRanges.DrawArgsBufferNumBytes + m_dataRanges.DrawArgsBufferOffset, 256 );
    m_dataRanges.IndirectBufferNumBytes = sizeof( DrawIndexedIndirectCommand ) * maxIndirectCommands;

    BufferDesc globalDataBufferDesc{ };
    globalDataBufferDesc.Descriptor = ResourceDescriptor::Buffer;
    globalDataBufferDesc.Usages     = ResourceUsage::VertexAndConstantBuffer;
    globalDataBufferDesc.HeapType   = HeapType::CPU_GPU;
    globalDataBufferDesc.NumBytes   = sizeof( GPUGlobalData );

    BufferDesc stagingBufferDesc{ };
    stagingBufferDesc.Descriptor = ResourceDescriptor::Buffer;
    stagingBufferDesc.Usages     = ResourceUsage::CopySrc | ResourceUsage::CopyDst;
    stagingBufferDesc.HeapType   = HeapType::CPU_GPU;
    size_t totalStagingSize      = m_dataRanges.IndirectBufferOffset + m_dataRanges.IndirectBufferNumBytes;
    stagingBufferDesc.NumBytes   = DataUtilities::Align( totalStagingSize, 256 );

    for ( size_t i = 0; i < m_frames.size( ); ++i )
    {
        m_frames[ i ]->StagingBuffer                = std::unique_ptr<IBufferResource>( m_logicalDevice->CreateBufferResource( stagingBufferDesc ) );
        m_frames[ i ]->StagingBufferMappedMemory    = static_cast<Byte *>( m_frames[ i ]->StagingBuffer->MapMemory( ) );
        m_frames[ i ]->GlobalDataBuffer             = std::unique_ptr<IBufferResource>( m_logicalDevice->CreateBufferResource( globalDataBufferDesc ) );
        m_frames[ i ]->GlobalDataBufferMappedMemory = static_cast<Byte *>( m_frames[ i ]->GlobalDataBuffer->MapMemory( ) );

        StructuredBufferDesc bufferDesc{ };
        bufferDesc.NumElements        = maxNumObjects;
        bufferDesc.Stride             = sizeof( GPUObjectData );
        m_frames[ i ]->ObjectBuffer   = CreateStructuredBuffer( bufferDesc );
        bufferDesc.NumElements        = maxNumMaterials;
        bufferDesc.Stride             = sizeof( MaterialData );
        m_frames[ i ]->MaterialBuffer = CreateStructuredBuffer( bufferDesc );
        bufferDesc.NumElements        = maxNumMeshes;
        bufferDesc.Stride             = sizeof( GPUMeshData );
        m_frames[ i ]->MeshBuffer     = CreateStructuredBuffer( bufferDesc );
        bufferDesc.NumElements        = maxNumInstances;
        bufferDesc.Stride             = sizeof( GPUInstanceData );
        m_frames[ i ]->InstanceBuffer = CreateStructuredBuffer( bufferDesc );

        bufferDesc.NumElements        = uploadDesc.MaxObjects;
        bufferDesc.Stride             = sizeof( DrawArguments );
        m_frames[ i ]->DrawArgsBuffer = CreateStructuredBuffer( bufferDesc );

        BufferDesc indirectBufferDesc{ };
        indirectBufferDesc.Descriptor = ResourceDescriptor::Buffer | ResourceDescriptor::IndirectBuffer;
        indirectBufferDesc.Usages     = ResourceUsage::IndirectArgument | ResourceUsage::CopyDst;
        indirectBufferDesc.HeapType   = HeapType::GPU;
        indirectBufferDesc.NumBytes   = m_dataRanges.IndirectBufferNumBytes;
        m_frames[ i ]->IndirectBuffer = std::unique_ptr<IBufferResource>( m_logicalDevice->CreateBufferResource( indirectBufferDesc ) );
    }
}

ISemaphore *GPUDrivenDataUpload::UpdateFrame( const uint32_t frameIndex ) const
{
    UpdateStagingBuffer( frameIndex );
    UpdateGlobalDataBuffer( frameIndex );

    const ICommandListArray commandLists = m_frames[ frameIndex ]->CommandLists;
    const auto             &frameData    = m_frames[ frameIndex ];

    size_t        commandListIndex = 0;
    ICommandList *commandList      = commandLists.Elements[ commandListIndex ];

    commandList->Begin( );
    CopyBufferRegionDesc copyRegionDesc{ };
    copyRegionDesc.SrcBuffer = frameData->StagingBuffer.get( );
    copyRegionDesc.DstBuffer = frameData->ObjectBuffer.get( );
    copyRegionDesc.SrcOffset = m_dataRanges.ObjectBufferOffset;
    copyRegionDesc.DstOffset = 0;
    copyRegionDesc.NumBytes  = m_dataRanges.ObjectBufferNumBytes;
    commandList->CopyBufferRegion( copyRegionDesc );
    commandList->End( );

    commandListIndex++;
    commandList = commandLists.Elements[ commandListIndex ];

    commandList->Begin( );
    copyRegionDesc.SrcBuffer = frameData->StagingBuffer.get( );
    copyRegionDesc.DstBuffer = frameData->MaterialBuffer.get( );
    copyRegionDesc.SrcOffset = m_dataRanges.MaterialBufferOffset;
    copyRegionDesc.DstOffset = 0;
    copyRegionDesc.NumBytes  = m_dataRanges.MaterialBufferNumBytes;
    commandList->CopyBufferRegion( copyRegionDesc );
    commandList->End( );

    commandListIndex++;
    commandList = commandLists.Elements[ commandListIndex ];
    commandList->Begin( );
    copyRegionDesc.SrcBuffer = frameData->StagingBuffer.get( );
    copyRegionDesc.DstBuffer = frameData->MeshBuffer.get( );
    copyRegionDesc.SrcOffset = m_dataRanges.MeshBufferOffset;
    copyRegionDesc.DstOffset = 0;
    copyRegionDesc.NumBytes  = m_dataRanges.MeshBufferNumBytes;
    commandList->CopyBufferRegion( copyRegionDesc );
    commandList->End( );

    commandListIndex++;
    commandList = commandLists.Elements[ commandListIndex ];
    commandList->Begin( );
    copyRegionDesc.SrcBuffer = frameData->StagingBuffer.get( );
    copyRegionDesc.DstBuffer = frameData->InstanceBuffer.get( );
    copyRegionDesc.SrcOffset = m_dataRanges.InstanceBufferOffset;
    copyRegionDesc.DstOffset = 0;
    copyRegionDesc.NumBytes  = m_dataRanges.InstanceBufferNumBytes;
    commandList->CopyBufferRegion( copyRegionDesc );
    commandList->End( );

    commandListIndex++;
    commandList = commandLists.Elements[ commandListIndex ];
    commandList->Begin( );
    copyRegionDesc.SrcBuffer = frameData->StagingBuffer.get( );
    copyRegionDesc.DstBuffer = frameData->DrawArgsBuffer.get( );
    copyRegionDesc.SrcOffset = m_dataRanges.DrawArgsBufferOffset;
    copyRegionDesc.DstOffset = 0;
    copyRegionDesc.NumBytes  = m_dataRanges.DrawArgsBufferNumBytes;
    commandList->CopyBufferRegion( copyRegionDesc );
    commandList->End( );

    commandListIndex++;
    commandList = commandLists.Elements[ commandListIndex ];
    commandList->Begin( );
    copyRegionDesc.SrcBuffer = frameData->StagingBuffer.get( );
    copyRegionDesc.DstBuffer = frameData->IndirectBuffer.get( );
    copyRegionDesc.SrcOffset = m_dataRanges.IndirectBufferOffset;
    copyRegionDesc.DstOffset = 0;
    copyRegionDesc.NumBytes  = m_dataRanges.IndirectBufferNumBytes;
    commandList->CopyBufferRegion( copyRegionDesc );
    commandList->End( );

    Submit( frameData->OnComplete.get( ), commandLists );
    return frameData->OnComplete.get( );
}

void GPUDrivenDataUpload::UpdateStagingBuffer( const uint32_t frameIndex ) const
{
    Byte *mappedMemory = m_frames[ frameIndex ]->StagingBufferMappedMemory;

    const size_t totalSize = m_dataRanges.IndirectBufferOffset + m_dataRanges.IndirectBufferNumBytes;
    memset( mappedMemory, 0, totalSize );

    auto *objectData   = reinterpret_cast<GPUObjectData *>( mappedMemory + m_dataRanges.ObjectBufferOffset );
    auto *materialData = reinterpret_cast<GPUMaterialData *>( mappedMemory + m_dataRanges.MaterialBufferOffset );
    auto *meshData     = reinterpret_cast<GPUMeshData *>( mappedMemory + m_dataRanges.MeshBufferOffset );
    auto *instanceData = reinterpret_cast<GPUInstanceData *>( mappedMemory + m_dataRanges.InstanceBufferOffset );
    auto *drawArgsData = reinterpret_cast<DrawArguments *>( mappedMemory + m_dataRanges.DrawArgsBufferOffset );
    auto *indirectData = reinterpret_cast<DrawIndexedIndirectCommand *>( mappedMemory + m_dataRanges.IndirectBufferOffset );

    uint32_t objectIndex   = 0;
    uint32_t materialIndex = 0;
    uint32_t meshIndex     = 0;
    uint32_t instanceIndex = 0;

    std::unordered_map<MeshHandle, uint32_t>     meshHandleToId;
    std::unordered_map<MaterialHandle, uint32_t> materialHandleToId;

    const auto &world = m_world->GetWorld( );

    materialData[ 0 ].BaseColorFactor          = { 1.0f, 1.0f, 1.0f, 1.0f };
    materialData[ 0 ].MetallicFactor           = 0.0f;
    materialData[ 0 ].RoughnessFactor          = 0.5f;
    materialData[ 0 ].NormalScale              = 1.0f;
    materialData[ 0 ].OcclusionStrength        = 1.0f;
    materialData[ 0 ].EmissiveFactor           = { 0.0f, 0.0f, 0.0f, 0.0f };
    materialData[ 0 ].BaseColorTexture         = 0;
    materialData[ 0 ].NormalTexture            = 0;
    materialData[ 0 ].MetallicRoughnessTexture = 0;
    materialData[ 0 ].OcclusionTexture         = 0;
    materialData[ 0 ].EmissiveTexture          = 0;
    materialData[ 0 ].CustomTexture0           = 0;
    materialData[ 0 ].CustomTexture1           = 0;
    materialData[ 0 ].Flags                    = 0;
    materialIndex                              = 1;

    meshData[ 0 ].VertexOffset = 0;
    meshData[ 0 ].IndexOffset  = 0;
    meshData[ 0 ].IndexCount   = 0;
    meshData[ 0 ].VertexCount  = 0;
    meshData[ 0 ].AABBMin      = { -1.0f, -1.0f, -1.0f };
    meshData[ 0 ].AABBMax      = { 1.0f, 1.0f, 1.0f };
    meshData[ 0 ].Padding0     = 0.0f;
    meshData[ 0 ].Padding1     = 0.0f;
    meshIndex                  = 1;

    const auto meshQuery = world.query<const MeshComponent, const RenderableComponent>( );
    meshQuery.each(
        [ & ]( const MeshComponent &mesh, const RenderableComponent &renderable )
        {
            if ( !renderable.Visible || mesh.Handle.Id == AssetHandle<TMeshHandle>::Invalid )
            {
                return;
            }
            if ( mesh.BatchId != m_batchId )
            {
                return;
            }

            if ( !meshHandleToId.contains( mesh.Handle ) )
            {
                if ( meshIndex >= m_uploadDesc.MaxMeshes )
                {
                    return;
                }

                if ( const GPUSubMesh gpuSubMesh = m_assets->Mesh( mesh.BatchId )->GetSubMesh( mesh.Handle ); gpuSubMesh.Metadata )
                {
                    meshData[ meshIndex ].VertexOffset = static_cast<uint32_t>( gpuSubMesh.VertexBuffer.Offset / sizeof( StaticMeshVertex ) );
                    meshData[ meshIndex ].IndexOffset  = static_cast<uint32_t>( gpuSubMesh.IndexBuffer.Offset / sizeof( uint32_t ) );
                    meshData[ meshIndex ].IndexCount   = gpuSubMesh.Metadata->NumIndices;
                    meshData[ meshIndex ].VertexCount  = gpuSubMesh.Metadata->NumVertices;

                    if ( !gpuSubMesh.Metadata->BoundingVolumes.empty( ) )
                    {
                        const auto &bounds            = gpuSubMesh.Metadata->BoundingVolumes[ 0 ];
                        meshData[ meshIndex ].AABBMin = bounds.Box.Min;
                        meshData[ meshIndex ].AABBMax = bounds.Box.Max;
                    }
                    else
                    {
                        meshData[ meshIndex ].AABBMin = { -1.0f, -1.0f, -1.0f };
                        meshData[ meshIndex ].AABBMax = { 1.0f, 1.0f, 1.0f };
                    }

                    meshData[ meshIndex ].Padding0 = 0.0f;
                    meshData[ meshIndex ].Padding1 = 0.0f;

                    meshHandleToId[ mesh.Handle ] = meshIndex;
                    meshIndex++;
                }
            }
        } );

    const auto materialQuery = world.query<const MaterialComponent>( );
    materialQuery.each(
        [ & ]( const MaterialComponent &materialComp )
        {
            if ( materialIndex >= m_uploadDesc.MaxMaterials || !materialComp.Handle.IsValid( ) )
                return;

            if ( !materialHandleToId.contains( materialComp.Handle ) )
            {
                if ( const auto materialData_ptr = m_assets->Material( )->GetMaterial( materialComp.Handle ) )
                {
                    materialData[ materialIndex ].BaseColorFactor   = materialData_ptr->BaseColorFactor;
                    materialData[ materialIndex ].MetallicFactor    = materialData_ptr->MetallicFactor;
                    materialData[ materialIndex ].RoughnessFactor   = materialData_ptr->RoughnessFactor;
                    materialData[ materialIndex ].NormalScale       = materialData_ptr->NormalScale;
                    materialData[ materialIndex ].OcclusionStrength = materialData_ptr->OcclusionStrength;
                    materialData[ materialIndex ].EmissiveFactor    = materialData_ptr->EmissiveFactor;

                    materialData[ materialIndex ].BaseColorTexture         = materialData_ptr->Albedo.IsValid( ) ? materialData_ptr->Albedo.Id : 0;
                    materialData[ materialIndex ].NormalTexture            = materialData_ptr->Normal.IsValid( ) ? materialData_ptr->Normal.Id : 0;
                    materialData[ materialIndex ].MetallicRoughnessTexture = materialData_ptr->Metallic.IsValid( ) ? materialData_ptr->Metallic.Id : 0;
                    materialData[ materialIndex ].OcclusionTexture         = materialData_ptr->Occlusion.IsValid( ) ? materialData_ptr->Occlusion.Id : 0;
                    materialData[ materialIndex ].EmissiveTexture          = materialData_ptr->Emissive.IsValid( ) ? materialData_ptr->Emissive.Id : 0;
                    materialData[ materialIndex ].CustomTexture0           = materialData_ptr->Custom0.IsValid( ) ? materialData_ptr->Custom0.Id : 0;
                    materialData[ materialIndex ].CustomTexture1           = materialData_ptr->Custom1.IsValid( ) ? materialData_ptr->Custom1.Id : 0;
                    materialData[ materialIndex ].Flags                    = 0;

                    materialHandleToId[ materialComp.Handle ] = materialIndex;
                    materialIndex++;
                }
            }
        } );

    const auto renderQuery = world.query<const TransformComponent, const MeshComponent, const RenderableComponent>( );
    renderQuery.each(
        [ & ]( const flecs::entity e, const TransformComponent &transform, const MeshComponent &mesh, const RenderableComponent &renderable )
        {
            if ( !renderable.Visible || objectIndex >= m_uploadDesc.MaxObjects )
            {
                return;
            }
            if ( mesh.BatchId != m_batchId )
            {
                return;
            }

            const DirectX::XMVECTOR scale          = MathConverter::Float3ToXMVECTOR( transform.Scale );
            const DirectX::XMVECTOR position       = MathConverter::Float3ToXMVECTOR( transform.Position );
            const DirectX::XMVECTOR rotationVec    = MathConverter::Float4ToXMVECTOR( transform.Rotation );
            const DirectX::XMMATRIX scaleMatrix    = DirectX::XMMatrixScalingFromVector( scale );
            const DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationQuaternion( rotationVec );
            const DirectX::XMMATRIX positionMatrix = DirectX::XMMatrixTranslationFromVector( position );
            const DirectX::XMMATRIX modelMatrix    = scaleMatrix * rotationMatrix * positionMatrix;
            objectData[ objectIndex ].ModelMatrix  = MathConverter::Float4X4FromXMMATRIX( modelMatrix );

            uint32_t materialId = 0;
            if ( e.has<MaterialComponent>( ) )
            {
                const auto materialComp = e.get<MaterialComponent>( );
                if ( const auto matIt = materialHandleToId.find( materialComp.Handle ); matIt != materialHandleToId.end( ) )
                {
                    materialId = matIt->second;
                }
            }
            objectData[ objectIndex ].MaterialID = materialId;

            const auto meshIt                = meshHandleToId.find( mesh.Handle );
            objectData[ objectIndex ].MeshID = meshIt != meshHandleToId.end( ) ? meshIt->second : 0;
            if ( const GPUSubMesh gpuSubMesh = m_assets->Mesh( mesh.BatchId )->GetSubMesh( mesh.Handle ); gpuSubMesh.Metadata->BoundingVolumes.size( ) > 0 )
            {
                const SphereBoundingVolume boundingVolume = gpuSubMesh.Metadata->BoundingVolumes[ 0 ].Sphere;
                objectData[ objectIndex ].BoundingSphere  = { boundingVolume.Center.X, boundingVolume.Center.Y, boundingVolume.Center.Z, boundingVolume.Radius };
            }
            uint32_t flags = 0;
            if ( renderable.CastShadows )
            {
                flags |= 1;
            }
            if ( renderable.ReceiveShadows )
            {
                flags |= 2;
            }
            objectData[ objectIndex ].Flags      = flags;
            objectData[ objectIndex ].CustomData = 0;

            instanceData[ instanceIndex ].ObjectID   = objectIndex;
            instanceData[ instanceIndex ].BatchIndex = 0;
            instanceData[ instanceIndex ].Padding    = Float2{ 0.0f, 0.0f };

            objectIndex++;
            instanceIndex++;
        } );

    struct MeshInstanceGroup
    {
        uint32_t              meshId;
        uint32_t              materialId;
        std::vector<uint32_t> instanceIndices;
    };

    std::unordered_map<uint32_t, MeshInstanceGroup> meshGroups;
    for ( uint32_t i = 0; i < objectIndex; ++i )
    {
        const uint32_t meshId     = objectData[ i ].MeshID;
        const uint32_t materialId = objectData[ i ].MaterialID;

        auto &group = meshGroups[ meshId ];
        if ( group.instanceIndices.empty( ) )
        {
            group.meshId     = meshId;
            group.materialId = materialId;
        }
        group.instanceIndices.push_back( i );
    }

    uint32_t drawArgsIndex         = 0;
    uint32_t currentInstanceOffset = 0;
    for ( const auto &group : meshGroups | std::views::values )
    {
        if ( drawArgsIndex >= m_uploadDesc.MaxObjects )
        {
            break;
        }
        drawArgsData[ drawArgsIndex ].MeshID         = group.meshId;
        drawArgsData[ drawArgsIndex ].MaterialID     = group.materialId;
        drawArgsData[ drawArgsIndex ].InstanceOffset = currentInstanceOffset;
        drawArgsData[ drawArgsIndex ].InstanceCount  = static_cast<uint32_t>( group.instanceIndices.size( ) );

        for ( uint32_t instanceIdx = 0; instanceIdx < group.instanceIndices.size( ); ++instanceIdx )
        {
            const uint32_t originalInstanceIndex = group.instanceIndices[ instanceIdx ];
            if ( currentInstanceOffset + instanceIdx < m_uploadDesc.MaxObjects )
            {
                instanceData[ currentInstanceOffset + instanceIdx ] = instanceData[ originalInstanceIndex ];
            }
        }

        currentInstanceOffset += static_cast<uint32_t>( group.instanceIndices.size( ) );
        drawArgsIndex++;
    }

    for ( uint32_t i = 0; i < drawArgsIndex; ++i )
    {
        if ( const uint32_t meshId = drawArgsData[ i ].MeshID; meshId < meshIndex && meshData[ meshId ].IndexCount > 0 )
        {
            indirectData[ i ].NumIndices    = meshData[ meshId ].IndexCount;
            indirectData[ i ].NumInstances  = drawArgsData[ i ].InstanceCount;
            indirectData[ i ].FirstIndex    = meshData[ meshId ].IndexOffset;
            indirectData[ i ].VertexOffset  = meshData[ meshId ].VertexOffset;
            indirectData[ i ].FirstInstance = drawArgsData[ i ].InstanceOffset;
        }
        else
        {
            indirectData[ i ].NumIndices    = 0;
            indirectData[ i ].NumInstances  = 0;
            indirectData[ i ].FirstIndex    = 0;
            indirectData[ i ].VertexOffset  = 0;
            indirectData[ i ].FirstInstance = 0;
        }
    }

    m_frames[ frameIndex ]->NumDraws = drawArgsIndex;
}

void GPUDrivenDataUpload::UpdateGlobalDataBuffer( const uint32_t frameIndex ) const
{
    Byte *mappedMemory = m_frames[ frameIndex ]->GlobalDataBufferMappedMemory;
    auto *globalData   = reinterpret_cast<GPUGlobalData *>( mappedMemory );

    const auto &world = m_world->GetWorld( );

    Float4x4 viewMatrix     = { };
    Float4x4 projMatrix     = { };
    Float4x4 viewProjMatrix = { };
    auto     cameraPos      = Float4{ 0.0f, 0.0f, 0.0f, 1.0f };

    const auto cameraQuery = world.query<const TransformComponent, const CameraComponent>( );
    cameraQuery.each(
        [ & ]( const TransformComponent &transform, const CameraComponent &camera )
        {
            if ( camera.Active )
            {
                viewMatrix     = camera.View;
                projMatrix     = camera.Projection;
                viewProjMatrix = camera.ViewProjection;
                cameraPos      = Float4{ transform.Position.X, transform.Position.Y, transform.Position.Z, 1.0f };
            }
        } );

    globalData->ViewMatrix     = viewMatrix;
    globalData->ProjMatrix     = projMatrix;
    globalData->ViewProjMatrix = viewProjMatrix;
    globalData->CameraPosition = cameraPos;

    globalData->FrustumPlanes[ 0 ] = Float4{ viewProjMatrix._14 + viewProjMatrix._11, viewProjMatrix._24 + viewProjMatrix._21, viewProjMatrix._34 + viewProjMatrix._31,
                                             viewProjMatrix._44 + viewProjMatrix._41 };
    globalData->FrustumPlanes[ 1 ] = Float4{ viewProjMatrix._14 - viewProjMatrix._11, viewProjMatrix._24 - viewProjMatrix._21, viewProjMatrix._34 - viewProjMatrix._31,
                                             viewProjMatrix._44 - viewProjMatrix._41 };
    globalData->FrustumPlanes[ 2 ] = Float4{ viewProjMatrix._14 + viewProjMatrix._12, viewProjMatrix._24 + viewProjMatrix._22, viewProjMatrix._34 + viewProjMatrix._32,
                                             viewProjMatrix._44 + viewProjMatrix._42 };
    globalData->FrustumPlanes[ 3 ] = Float4{ viewProjMatrix._14 - viewProjMatrix._12, viewProjMatrix._24 - viewProjMatrix._22, viewProjMatrix._34 - viewProjMatrix._32,
                                             viewProjMatrix._44 - viewProjMatrix._42 };
    globalData->FrustumPlanes[ 4 ] = Float4{ viewProjMatrix._14 + viewProjMatrix._13, viewProjMatrix._24 + viewProjMatrix._23, viewProjMatrix._34 + viewProjMatrix._33,
                                             viewProjMatrix._44 + viewProjMatrix._43 };
    globalData->FrustumPlanes[ 5 ] = Float4{ viewProjMatrix._14 - viewProjMatrix._13, viewProjMatrix._24 - viewProjMatrix._23, viewProjMatrix._34 - viewProjMatrix._33,
                                             viewProjMatrix._44 - viewProjMatrix._43 };

    for ( int i = 0; i < 6; ++i )
    {
        const float length = std::sqrt( globalData->FrustumPlanes[ i ].X * globalData->FrustumPlanes[ i ].X + globalData->FrustumPlanes[ i ].Y * globalData->FrustumPlanes[ i ].Y +
                                        globalData->FrustumPlanes[ i ].Z * globalData->FrustumPlanes[ i ].Z );
        if ( length > 0.0f )
        {
            globalData->FrustumPlanes[ i ].X /= length;
            globalData->FrustumPlanes[ i ].Y /= length;
            globalData->FrustumPlanes[ i ].Z /= length;
            globalData->FrustumPlanes[ i ].W /= length;
        }
    }

    globalData->ScreenSize = Float2{ 1920.0f, 1080.0f };
    globalData->Time       = 0.0f;
    globalData->DeltaTime  = 0.016f;
}

void GPUDrivenDataUpload::Submit( ISemaphore *onComplete, const ICommandListArray &commandListsToSubmit ) const
{
    ExecuteCommandListsDesc executeCommandListsDesc{ };
    executeCommandListsDesc.CommandLists                 = commandListsToSubmit;
    executeCommandListsDesc.SignalSemaphores.Elements    = &onComplete;
    executeCommandListsDesc.SignalSemaphores.NumElements = 1;
    m_copyQueue->ExecuteCommandLists( executeCommandListsDesc );
}

GPUDrivenBuffers GPUDrivenDataUpload::GetBuffers( const uint32_t frameIndex ) const
{
    GPUDrivenBuffers buffers{ };
    buffers.GlobalDataBuffer = m_frames[ frameIndex ]->GlobalDataBuffer.get( );
    buffers.ObjectBuffer     = m_frames[ frameIndex ]->ObjectBuffer.get( );
    buffers.MaterialBuffer   = m_frames[ frameIndex ]->MaterialBuffer.get( );
    buffers.MeshBuffer       = m_frames[ frameIndex ]->MeshBuffer.get( );
    buffers.InstanceBuffer   = m_frames[ frameIndex ]->InstanceBuffer.get( );
    buffers.DrawArgsBuffer   = m_frames[ frameIndex ]->DrawArgsBuffer.get( );
    buffers.IndirectBuffer   = m_frames[ frameIndex ]->IndirectBuffer.get( );
    return buffers;
}

uint32_t GPUDrivenDataUpload::GetNumDraws( const uint32_t frameIndex ) const
{
    return m_frames[ frameIndex ]->NumDraws;
}

GPUDrivenDataUpload::~GPUDrivenDataUpload( )
{
    for ( const auto &frame : m_frames )
    {
        frame->StagingBuffer->UnmapMemory( );
        frame->GlobalDataBuffer->UnmapMemory( );
    }
}

std::unique_ptr<IBufferResource> GPUDrivenDataUpload::CreateStructuredBuffer( const StructuredBufferDesc &structDesc ) const
{
    BufferDesc bufferDesc{ };
    bufferDesc.Descriptor    = ResourceDescriptor::StructuredBuffer;
    bufferDesc.Usages        = ResourceUsage::ShaderResource;
    bufferDesc.HeapType      = HeapType::GPU;
    bufferDesc.NumBytes      = structDesc.NumElements * structDesc.Stride;
    bufferDesc.Alignment     = structDesc.Stride;
    bufferDesc.StructureDesc = structDesc;

    return std::unique_ptr<IBufferResource>( m_logicalDevice->CreateBufferResource( bufferDesc ) );
}
