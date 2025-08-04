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

#include <cstdint>
#include "DenOfIzGraphics/Utilities/InteropMath.h"

using namespace DenOfIz;

namespace DZEngine
{
    constexpr uint32_t MaxNumTextures = 1024;

    struct GPUMaterialData
    {
        Float4 BaseColorFactor;

        float MetallicFactor;
        float RoughnessFactor;
        float NormalScale;
        float OcclusionStrength;

        Float4 EmissiveFactor;

        uint32_t BaseColorTexture;
        uint32_t NormalTexture;
        uint32_t MetallicRoughnessTexture;
        uint32_t OcclusionTexture;
        uint32_t EmissiveTexture;

        uint32_t Flags;
        uint32_t CustomTexture0;
        uint32_t CustomTexture1;
    };

    struct GPUObjectData
    {
        Float4x4 ModelMatrix;

        Float4 BoundingSphere;

        uint32_t MaterialID;
        uint32_t MeshID;
        uint32_t Flags;
        uint32_t CustomData;
    };

    struct GPUMeshData
    {
        uint32_t VertexOffset;
        uint32_t IndexOffset;
        uint32_t IndexCount;
        uint32_t VertexCount;

        Float3 AABBMin;
        float  Padding0;
        Float3 AABBMax;
        float  Padding1;
    };

    struct DrawIndexedIndirectCommand
    {
        uint32_t NumIndices;
        uint32_t NumInstances;
        uint32_t FirstIndex;
        int32_t  VertexOffset;
        uint32_t FirstInstance;
    };

    struct DrawCommand
    {
        DrawIndexedIndirectCommand DrawCmd;
        uint32_t                   BatchID;
        uint32_t                   MaterialID;
        uint32_t                   MeshID;
        uint32_t                   Padding;
    };

    struct BatchData
    {
        uint32_t MaterialID;
        uint32_t MeshID;
        uint32_t InstanceCount;
        uint32_t FirstInstance;
    };

    struct GPUInstanceData
    {
        uint32_t ObjectID;
        uint32_t BatchIndex;
        Float2   Padding;
    };

    struct GPUGlobalData
    {
        Float4x4 ViewMatrix;
        Float4x4 ProjMatrix;
        Float4x4 ViewProjMatrix;
        Float4   CameraPosition;
        Float4   FrustumPlanes[ 6 ];
        Float2   ScreenSize;
        float    Time;
        float    DeltaTime;
    };

    struct DrawArguments
    {
        uint32_t MeshID;
        uint32_t MaterialID;
        uint32_t InstanceOffset;
        uint32_t InstanceCount;
    };
} // namespace DZEngine
