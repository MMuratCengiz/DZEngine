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

#include "DenOfIzGraphics/Backends/Interface/ILogicalDevice.h"
#include "GPUBufferView.h"

namespace DenOfIz
{
    struct DZ_API AsyncGpuUploadDesc
    {
        ILogicalDevice *Device;
    };

    struct DZ_API GpuSceneHandle{

    };

    struct DZ_API GpuSceneDesc
    {
        size_t SceneId;
        size_t ReservedVertexBytes;
        size_t ReservedIndexBytes;
    };

    struct DZ_API GpuUploadMeshDesc
    {
        BinaryReader *Reader;
    };

    struct DZ_API GpuUploadHandle
    {
        size_t Id;
    };

    enum class GpuUploadStatus
    {
        Pending,
        Completed,
        Failed
    };

    struct DZ_API GpuUploadMeshResult
    {
        GPUBufferView   VertexBuffer;
        GPUBufferView   IndexBuffer;
        GpuUploadStatus Status;
    };

    class AsyncGpuUpload
    {
        ILogicalDevice   *m_device;
        ICommandQueue    *m_commandQueue;
        ICommandListPool *m_commandListPool;

        struct GpuMeshData
        {
            GpuUploadHandle UploadHandle;
            GPUBufferView   VertexBuffer;
            GPUBufferView   IndexBuffer;
            GpuUploadStatus Status;
        };

        struct GpuSceneData
        {
            IBufferResource         *VertexBuffer;
            IBufferResource         *IndexBuffer;
            size_t                   UsedVertexBytes;
            size_t                   UsedIndexBytes;
            size_t                   ReservedVertexBytes;
            size_t                   ReservedIndexBytes;
            size_t                   SceneId;
            std::vector<GpuMeshData> Meshes;
        };
        std::vector<IBufferResource *> m_sceneBuffers;

    public:
        explicit AsyncGpuUpload( const AsyncGpuUploadDesc &desc );

        GpuSceneHandle CreateScene( const GpuSceneDesc &desc );
        void           GetScene( const GpuSceneHandle &handle );
        void           DestroyScene( const GpuSceneHandle &handle );

        GpuUploadHandle     UploadMesh( const GpuUploadMeshDesc &desc );
        GpuUploadMeshResult GetMesh( const GpuUploadHandle &handle );

        ~AsyncGpuUpload( ) = default;
    };
} // namespace DenOfIz
