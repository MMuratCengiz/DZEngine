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

#include <string>
#include <vector>
#include "DZEngine/Components/AssetHandle.h"
#include "DenOfIzGraphics/Assets/Serde/Animation/AnimationAsset.h"

using namespace DenOfIz;

namespace DZEngine
{
    struct AnimationKeyframe
    {
        float  Timestamp;
        Float3 Position;
        Float4 Rotation;
        Float3 Scale;
    };

    struct JointAnimationTrack
    {
        std::string                    Name;
        std::vector<AnimationKeyframe> Keyframes;
    };

    struct MorphAnimationTrack
    {
        std::string                          Name;
        std::vector<std::pair<float, float>> WeightKeyframes; // timestamp, weight
    };

    struct AnimationClipData
    {
        std::string                      Name;
        float                            Duration = 0.0f;
        std::vector<JointAnimationTrack> JointTracks;
        std::vector<MorphAnimationTrack> MorphTracks;
    };

    class AnimationAssetData
    {
    public:
        std::string                    Name;
        std::string                    SkeletonRef;
        std::vector<AnimationClipData> Clips;
        AnimationClipHandle            Handle;

        AnimationAssetData( ) = default;

        static AnimationAssetData LoadFromAnimationAsset( const AnimationAsset &animationAsset );

        size_t                   GetNumClips( ) const;
        const AnimationClipData *GetClip( const std::string &name ) const;
        float                    GetTotalDuration( ) const;
        bool                     HasSkeletonRef( ) const;
    };

} // namespace DZEngine
