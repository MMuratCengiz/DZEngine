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

#include "DZEngine/Assets/SkeletonAssetData.h"
#include <algorithm>

using namespace DZEngine;

SkeletonAssetData SkeletonAssetData::LoadFromSkeletonAsset( const SkeletonAsset &skeletonAsset )
{
    SkeletonAssetData data;
    data.Name = skeletonAsset.Name.Get( );
    data.Joints.reserve( skeletonAsset.Joints.NumElements );
    for ( uint32_t i = 0; i < skeletonAsset.Joints.NumElements; ++i )
    {
        const auto &srcJoint = skeletonAsset.Joints.Elements[ i ];
        JointData   joint;

        joint.Name              = srcJoint.Name.Get( );
        joint.InverseBindMatrix = srcJoint.InverseBindMatrix;
        joint.LocalTranslation  = srcJoint.LocalTranslation;
        joint.LocalRotationQuat = srcJoint.LocalRotationQuat;
        joint.LocalScale        = srcJoint.LocalScale;
        joint.GlobalTransform   = srcJoint.GlobalTransform;
        joint.Index             = srcJoint.Index;
        joint.ParentIndex       = srcJoint.ParentIndex;
        joint.ChildIndices.reserve( srcJoint.ChildIndices.NumElements );
        for ( uint32_t j = 0; j < srcJoint.ChildIndices.NumElements; ++j )
        {
            joint.ChildIndices.push_back( srcJoint.ChildIndices.Elements[ j ] );
        }

        data.Joints.push_back( std::move( joint ) );
    }

    return data;
}

size_t SkeletonAssetData::GetNumJoints( ) const
{
    return Joints.size( );
}

const JointData *SkeletonAssetData::GetJoint( const std::string &name ) const
{
    const auto it = std::ranges::find_if( Joints, [ &name ]( const JointData &joint ) { return joint.Name == name; } );
    return it != Joints.end( ) ? &*it : nullptr;
}

const JointData *SkeletonAssetData::GetJoint( const uint32_t index ) const
{
    if ( index >= Joints.size( ) )
    {
        return nullptr;
    }
    return &Joints[ index ];
}

int32_t SkeletonAssetData::FindJointIndex( const std::string &name ) const
{
    for ( size_t i = 0; i < Joints.size( ); ++i )
    {
        if ( Joints[ i ].Name == name )
        {
            return static_cast<int32_t>( i );
        }
    }
    return -1;
}

bool SkeletonAssetData::HasJoints( ) const
{
    return !Joints.empty( );
}
