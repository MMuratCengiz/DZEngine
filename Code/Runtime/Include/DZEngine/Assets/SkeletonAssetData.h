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
#include <string>
#include <vector>
#include "DZEngine/Components/AssetHandle.h"
#include "DenOfIzGraphics/Assets/Serde/Skeleton/SkeletonAsset.h"

using namespace DenOfIz;

namespace DZEngine
{
    struct JointData
    {
        std::string           Name;
        Float4x4              InverseBindMatrix;
        Float3                LocalTranslation;
        Float4                LocalRotationQuat;
        Float3                LocalScale;
        Float4x4              GlobalTransform;
        uint32_t              Index       = 0;
        int32_t               ParentIndex = -1;
        std::vector<uint32_t> ChildIndices;
    };

    class SkeletonAssetData
    {
    public:
        std::string            Name;
        std::vector<JointData> Joints;
        SkeletonHandle         Handle;

        SkeletonAssetData( ) = default;

        static SkeletonAssetData LoadFromSkeletonAsset( const SkeletonAsset &skeletonAsset );

        size_t           GetNumJoints( ) const;
        const JointData *GetJoint( const std::string &name ) const;
        const JointData *GetJoint( uint32_t index ) const;
        int32_t          FindJointIndex( const std::string &name ) const;
        bool             HasJoints( ) const;
    };

} // namespace DZEngine
