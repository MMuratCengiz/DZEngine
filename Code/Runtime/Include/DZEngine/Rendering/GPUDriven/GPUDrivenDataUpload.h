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

#include "DZEngine/Assets/AssetBatcher.h"
#include "DZEngine/Scene/Scene.h"
#include "DenOfIzGraphics/DenOfIzGraphics.h"
#include "GPUDrivenSceneData.h"

using namespace DenOfIz;

namespace DZEngine
{
    struct GPUDrivenDataUploadDesc
    {
        GraphicsContext *GraphicsContext;
        AssetBatcher    *Assets;
        World           *World;
        uint32_t         BatchId;
        uint32_t         NumFrames;
        uint32_t         MaxObjects   = 65536;
        uint32_t         MaxMaterials = 512;
        uint32_t         MaxMeshes    = 2048;
    };

    struct GPUDrivenBuffers
    {
        IBufferResource *GlobalDataBuffer;
        IBufferResource *ObjectBuffer;
        IBufferResource *MaterialBuffer;
        IBufferResource *MeshBuffer;
        IBufferResource *InstanceBuffer;
        IBufferResource *DrawArgsBuffer;
        IBufferResource *IndirectBuffer;
    };

    class GPUDrivenDataUpload
    {
        ILogicalDevice                *m_logicalDevice;
        Scene                         *m_scene;
        World                         *m_world;
        AssetBatcher                  *m_assets;
        size_t                         m_batchId;
        GPUDrivenDataUploadDesc        m_uploadDesc;
        std::unique_ptr<ICommandQueue> m_copyQueue;

        struct DataRanges
        {
            size_t ObjectBufferNumBytes;
            size_t ObjectBufferOffset;

            size_t MaterialBufferNumBytes;
            size_t MaterialBufferOffset;

            size_t MeshBufferNumBytes;
            size_t MeshBufferOffset;

            size_t InstanceBufferNumBytes;
            size_t InstanceBufferOffset;
            
            size_t DrawArgsBufferNumBytes;
            size_t DrawArgsBufferOffset;
            
            size_t IndirectBufferNumBytes;
            size_t IndirectBufferOffset;
        };

        struct FrameData
        {
            std::unique_ptr<ICommandListPool> CommandListPool;
            ICommandListArray                 CommandLists;

            std::unique_ptr<IBufferResource> StagingBuffer;
            Byte                            *StagingBufferMappedMemory;
            std::unique_ptr<IBufferResource> GlobalDataBuffer; // g_GlobalData
            Byte                            *GlobalDataBufferMappedMemory;

            std::unique_ptr<IBufferResource> ObjectBuffer;   // g_ObjectBuffer;
            std::unique_ptr<IBufferResource> MaterialBuffer; // g_MaterialBuffer;
            std::unique_ptr<IBufferResource> MeshBuffer;     // g_MeshBuffer;
            std::unique_ptr<IBufferResource> InstanceBuffer; // g_InstanceBuffer;
            std::unique_ptr<IBufferResource> DrawArgsBuffer; // g_DrawArgsBuffer;
            std::unique_ptr<IBufferResource> IndirectBuffer; // Indirect draw commands
            
            uint32_t NumDraws = 0;
        };

        DataRanges                              m_dataRanges;
        std::vector<std::unique_ptr<FrameData>> m_frames;

    public:
        explicit GPUDrivenDataUpload( const GPUDrivenDataUploadDesc &uploadDesc );
        void             UpdateFrame( ISemaphore *onComplete, uint32_t frameIndex ) const;
        void             UpdateStagingBuffer( uint32_t frameIndex ) const;
        void             UpdateGlobalDataBuffer( uint32_t frameIndex ) const;
        void             Submit( ISemaphore *onComplete, const ICommandListArray &commandListsToSubmit ) const;
        GPUDrivenBuffers GetBuffers( uint32_t frameIndex ) const;
        uint32_t         GetNumDraws( uint32_t frameIndex ) const;
        ~GPUDrivenDataUpload( ) = default;

    private:
        std::unique_ptr<IBufferResource> CreateStructuredBuffer( const StructuredBufferDesc &structDesc ) const;
    };
} // namespace DZEngine
