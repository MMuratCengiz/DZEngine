#pragma once

#include <algorithm>
#include <cstdint>
#include <string>
#include <vector>
#include "DenOfIzGraphics/Assets/Serde/Mesh/MeshAsset.h"

using namespace DenOfIz;

namespace DZEngine
{
    struct MorphTarget
    {
        std::string Name;
        float       DefaultWeight = 0.0f;
    };

    struct UVChannel
    {
        std::string SemanticName;
        uint32_t    Index = 0;
    };

    struct VertexAttributeConfig
    {
        uint32_t                 NumPositionComponents = 4;
        uint32_t                 NumUVAttributes       = 2;
        std::vector<UVChannel>   UVChannels;
        std::vector<ColorFormat> ColorFormats;
        uint32_t                 NumBoneInfluences = 4;
    };

    struct BoundingVolume
    {
        BoundingVolumeType Type = BoundingVolumeType::Box;
        std::string        Name;

        BoxBoundingVolume     Box;
        SphereBoundingVolume  Sphere;
        CapsuleBoundingVolume Capsule;
    };

    struct SubMeshData
    {
        std::string                 Name;
        PrimitiveTopology           Topology    = PrimitiveTopology::Triangle;
        uint64_t                    NumVertices = 0;
        uint64_t                    NumIndices  = 0;
        IndexType                   IndexType   = IndexType::Uint32;
        Float3                      MinBounds;
        Float3                      MaxBounds;
        std::string                 MaterialRef;
        std::vector<BoundingVolume> BoundingVolumes;
    };

    struct UserProperty
    {
        std::string Key;
        std::string Value;
    };

    class MeshAssetData
    {
    public:
        std::string                Name;
        VertexEnabledAttributes    EnabledAttributes;
        VertexAttributeConfig      AttributeConfig;
        std::vector<SubMeshData>   SubMeshes;
        MorphTargetDeltaAttributes MorphTargetDeltaAttributes;
        std::vector<MorphTarget>   MorphTargets;
        std::vector<std::string>   AnimationRefs;
        std::string                SkeletonRef;
        std::vector<UserProperty>  UserProperties;

        MeshAssetData( ) = default;

        static MeshAssetData LoadFromMeshAsset( const MeshAsset &meshAsset );
        size_t               GetVertexNumBytes( ) const;
        size_t               GetTotalNumVertices( ) const;
        size_t               GetTotalNumIndices( ) const;
        void                 GetBounds( Float3 &outMin, Float3 &outMax ) const;
        bool                 HasSkeleton( ) const;
        bool                 HasMorphTargets( ) const;
    };

} // namespace DZEngine
