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

#include "DZEngine/Assets/AnimationAssetData.h"
#include <algorithm>

using namespace DZEngine;

AnimationAssetData AnimationAssetData::LoadFromAnimationAsset( const AnimationAsset &animationAsset )
{
    AnimationAssetData data;
    data.Name        = animationAsset.Name.Get( );
    data.SkeletonRef = animationAsset.SkeletonRef.Path.Get( );
    data.Clips.reserve( animationAsset.Animations.NumElements );
    for ( uint32_t i = 0; i < animationAsset.Animations.NumElements; ++i )
    {
        const auto       &srcClip = animationAsset.Animations.Elements[ i ];
        AnimationClipData clipData;

        clipData.Name     = srcClip.Name.Get( );
        clipData.Duration = srcClip.Duration;

        clipData.JointTracks.reserve( srcClip.Tracks.NumElements );
        for ( uint32_t j = 0; j < srcClip.Tracks.NumElements; ++j )
        {
            const auto         &srcTrack = srcClip.Tracks.Elements[ j ];
            JointAnimationTrack track;
            track.Name = srcTrack.JointName.Get( );

            uint32_t maxKeyframes = std::max( { srcTrack.PositionKeys.NumElements, srcTrack.RotationKeys.NumElements, srcTrack.ScaleKeys.NumElements } );
            track.Keyframes.reserve( maxKeyframes );
            for ( uint32_t k = 0; k < maxKeyframes; ++k )
            {
                AnimationKeyframe keyframe;
                if ( k < srcTrack.PositionKeys.NumElements )
                {
                    keyframe.Timestamp = srcTrack.PositionKeys.Elements[ k ].Timestamp;
                    keyframe.Position  = srcTrack.PositionKeys.Elements[ k ].Value;
                }
                if ( k < srcTrack.RotationKeys.NumElements )
                {
                    keyframe.Timestamp = srcTrack.RotationKeys.Elements[ k ].Timestamp;
                    keyframe.Rotation  = srcTrack.RotationKeys.Elements[ k ].Value;
                }
                if ( k < srcTrack.ScaleKeys.NumElements )
                {
                    keyframe.Timestamp = srcTrack.ScaleKeys.Elements[ k ].Timestamp;
                    keyframe.Scale     = srcTrack.ScaleKeys.Elements[ k ].Value;
                }

                track.Keyframes.push_back( keyframe );
            }

            clipData.JointTracks.push_back( std::move( track ) );
        }
        clipData.MorphTracks.reserve( srcClip.MorphTracks.NumElements );
        for ( uint32_t j = 0; j < srcClip.MorphTracks.NumElements; ++j )
        {
            const auto         &srcMorphTrack = srcClip.MorphTracks.Elements[ j ];
            MorphAnimationTrack morphTrack;
            morphTrack.Name = srcMorphTrack.Name.Get( );

            morphTrack.WeightKeyframes.reserve( srcMorphTrack.Keyframes.NumElements );
            for ( uint32_t k = 0; k < srcMorphTrack.Keyframes.NumElements; ++k )
            {
                const auto &keyframe = srcMorphTrack.Keyframes.Elements[ k ];
                morphTrack.WeightKeyframes.emplace_back( keyframe.Timestamp, keyframe.Weight );
            }

            clipData.MorphTracks.push_back( std::move( morphTrack ) );
        }

        data.Clips.push_back( std::move( clipData ) );
    }
    return data;
}

size_t AnimationAssetData::GetNumClips( ) const
{
    return Clips.size( );
}

const AnimationClipData *AnimationAssetData::GetClip( const std::string &name ) const
{
    const auto it = std::ranges::find_if( Clips, [ &name ]( const AnimationClipData &clip ) { return clip.Name == name; } );
    return it != Clips.end( ) ? &*it : nullptr;
}

float AnimationAssetData::GetTotalDuration( ) const
{
    float totalDuration = 0.0f;
    for ( const auto &clip : Clips )
    {
        totalDuration = std::max( totalDuration, clip.Duration );
    }
    return totalDuration;
}

bool AnimationAssetData::HasSkeletonRef( ) const
{
    return !SkeletonRef.empty( );
}
