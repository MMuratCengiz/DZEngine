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

#include "DZEngine/Assets/MeshAssetData.h"

using namespace DZEngine;

MeshAssetData MeshAssetData::LoadFromMeshAsset( const MeshAsset &meshAsset )
{
    MeshAssetData data;

    data.Name = meshAsset.Name.Get( );

    data.EnabledAttributes.Position     = meshAsset.EnabledAttributes.Position;
    data.EnabledAttributes.Normal       = meshAsset.EnabledAttributes.Normal;
    data.EnabledAttributes.UV           = meshAsset.EnabledAttributes.UV;
    data.EnabledAttributes.Color        = meshAsset.EnabledAttributes.Color;
    data.EnabledAttributes.Tangent      = meshAsset.EnabledAttributes.Tangent;
    data.EnabledAttributes.Bitangent    = meshAsset.EnabledAttributes.Bitangent;
    data.EnabledAttributes.BlendIndices = meshAsset.EnabledAttributes.BlendIndices;
    data.EnabledAttributes.BlendWeights = meshAsset.EnabledAttributes.BlendWeights;

    data.AttributeConfig.NumPositionComponents   = meshAsset.AttributeConfig.NumPositionComponents;
    data.AttributeConfig.NumUVAttributes         = meshAsset.AttributeConfig.NumUVAttributes;
    data.AttributeConfig.NumBoneInfluences       = meshAsset.AttributeConfig.NumBoneInfluences;

    for ( uint32_t i = 0; i < meshAsset.AttributeConfig.UVChannels.NumElements; ++i )
    {
        UVChannel channel;
        channel.SemanticName = meshAsset.AttributeConfig.UVChannels.Elements[ i ].SemanticName.Get( );
        channel.Index        = meshAsset.AttributeConfig.UVChannels.Elements[ i ].Index;
        data.AttributeConfig.UVChannels.push_back( channel );
    }

    for ( uint32_t i = 0; i < meshAsset.AttributeConfig.ColorFormats.NumElements; ++i )
    {
        data.AttributeConfig.ColorFormats.push_back( meshAsset.AttributeConfig.ColorFormats.Elements[ i ] );
    }

    for ( uint32_t i = 0; i < meshAsset.SubMeshes.NumElements; ++i )
    {
        const auto &srcSubMesh = meshAsset.SubMeshes.Elements[ i ];
        SubMeshData subMesh;

        subMesh.Name        = srcSubMesh.Name.Get( );
        subMesh.Topology    = srcSubMesh.Topology;
        subMesh.NumVertices = srcSubMesh.NumVertices;
        subMesh.NumIndices  = srcSubMesh.NumIndices;
        subMesh.IndexType   = srcSubMesh.IndexType;
        subMesh.MinBounds   = srcSubMesh.MinBounds;
        subMesh.MaxBounds   = srcSubMesh.MaxBounds;
        subMesh.MaterialRef = srcSubMesh.MaterialRef.Path.Get( );

        for ( uint32_t j = 0; j < srcSubMesh.BoundingVolumes.NumElements; ++j )
        {
            const auto    &srcBV = srcSubMesh.BoundingVolumes.Elements[ j ];
            BoundingVolume bv;

            bv.Type    = srcBV.Type;
            bv.Name    = srcBV.Name.Get( );
            bv.Box     = srcBV.Box;
            bv.Sphere  = srcBV.Sphere;
            bv.Capsule = srcBV.Capsule;

            subMesh.BoundingVolumes.push_back( bv );
        }

        data.SubMeshes.push_back( subMesh );
    }

    data.MorphTargetDeltaAttributes.Position = meshAsset.MorphTargetDeltaAttributes.Position;
    data.MorphTargetDeltaAttributes.Normal   = meshAsset.MorphTargetDeltaAttributes.Normal;
    data.MorphTargetDeltaAttributes.Tangent  = meshAsset.MorphTargetDeltaAttributes.Tangent;

    for ( uint32_t i = 0; i < meshAsset.MorphTargets.NumElements; ++i )
    {
        MorphTarget target;
        target.Name          = meshAsset.MorphTargets.Elements[ i ].Name.Get( );
        target.DefaultWeight = meshAsset.MorphTargets.Elements[ i ].DefaultWeight;
        data.MorphTargets.push_back( target );
    }
    for ( uint32_t i = 0; i < meshAsset.AnimationRefs.NumElements; ++i )
    {
        data.AnimationRefs.push_back( meshAsset.AnimationRefs.Elements[ i ].Path.Get( ) );
    }
    data.SkeletonRef = meshAsset.SkeletonRef.Path.Get( );
    return data;
}

size_t MeshAssetData::GetVertexNumBytes( ) const
{
    uint32_t    numBytes = 0;
    if ( EnabledAttributes.Position )
    {
        numBytes += 4 * sizeof( float );
    }
    if ( EnabledAttributes.Normal )
    {
        numBytes += 4 * sizeof( float );
    }
    if ( EnabledAttributes.UV )
    {
        numBytes += AttributeConfig.NumUVAttributes * 2 * sizeof( float );
    }
    if ( EnabledAttributes.Color )
    {
        for ( const auto i : AttributeConfig.ColorFormats )
        {
            switch ( i )
            {
            case ColorFormat::RGBA:
                numBytes += 4 * sizeof( float );
                break;
            case ColorFormat::RGB:
                numBytes += 3 * sizeof( float );
                break;
            case ColorFormat::RG:
                numBytes += 2 * sizeof( float );
                break;
            case ColorFormat::R:
                numBytes += 1 * sizeof( float );
                break;
            }
        }
    }
    if ( EnabledAttributes.Tangent )
    {
        numBytes += 4 * sizeof( float );
    }
    if ( EnabledAttributes.Bitangent )
    {
        numBytes += 4 * sizeof( float );
    }
    if ( EnabledAttributes.BlendIndices )
    {
        numBytes += sizeof( uint32_t );
        numBytes += AttributeConfig.NumBoneInfluences * sizeof( uint32_t );
    }
    if ( EnabledAttributes.BlendWeights )
    {
        numBytes += AttributeConfig.NumBoneInfluences * sizeof( float );
    }
    return numBytes;
}

size_t MeshAssetData::GetTotalNumVertices( ) const
{
    size_t count = 0;
    for ( const auto &subMesh : SubMeshes )
    {
        count += subMesh.NumVertices;
    }
    return count;
}

size_t MeshAssetData::GetTotalNumIndices( ) const
{
    size_t count = 0;
    for ( const auto &subMesh : SubMeshes )
    {
        count += subMesh.NumIndices;
    }
    return count;
}

void MeshAssetData::GetBounds( Float3 &outMin, Float3 &outMax ) const
{
    if ( SubMeshes.empty( ) )
    {
        outMin = Float3{ 0, 0, 0 };
        outMax = Float3{ 0, 0, 0 };
        return;
    }

    outMin = SubMeshes[ 0 ].MinBounds;
    outMax = SubMeshes[ 0 ].MaxBounds;

    for ( size_t i = 1; i < SubMeshes.size( ); ++i )
    {
        const auto &bounds = SubMeshes[ i ];
        outMin.X           = std::min( outMin.X, bounds.MinBounds.X );
        outMin.Y           = std::min( outMin.Y, bounds.MinBounds.Y );
        outMin.Z           = std::min( outMin.Z, bounds.MinBounds.Z );
        outMax.X           = std::max( outMax.X, bounds.MaxBounds.X );
        outMax.Y           = std::max( outMax.Y, bounds.MaxBounds.Y );
        outMax.Z           = std::max( outMax.Z, bounds.MaxBounds.Z );
    }
}

bool MeshAssetData::HasSkeleton( ) const
{
    return !SkeletonRef.empty( ) && EnabledAttributes.BlendIndices && EnabledAttributes.BlendWeights;
}

bool MeshAssetData::HasMorphTargets( ) const
{
    return !MorphTargets.empty( );
}
