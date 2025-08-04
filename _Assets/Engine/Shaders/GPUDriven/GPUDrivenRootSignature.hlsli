struct GPUMaterialData
{
    float4 BaseColorFactor;
    float MetallicFactor;
    float RoughnessFactor;
    float NormalScale;
    float OcclusionStrength;
    float4 EmissiveFactor;
    uint BaseColorTexture;
    uint NormalTexture;
    uint MetallicRoughnessTexture;
    uint OcclusionTexture;
    uint EmissiveTexture;
    uint Flags;
    uint CustomTexture0;
    uint CustomTexture1;
};

struct GPUObjectData
{
    float4x4 ModelMatrix;
    float4 BoundingSphere;
    uint MaterialID;
    uint MeshID;
    uint Flags;
    uint CustomData;
};

struct GPUMeshData
{
    uint VertexOffset;
    uint IndexOffset;
    uint IndexCount;
    uint VertexCount;
    float3 AABBMin;
    float Padding0;
    float3 AABBMax;
    float Padding1;
};

struct GPUGlobalData
{
    float4x4 ViewMatrix;
    float4x4 ProjMatrix;
    float4x4 ViewProjMatrix;
    float4 CameraPosition;
    float4 FrustumPlanes[6];
    float2 ScreenSize;
    float Time;
    float DeltaTime;
};

struct GPUInstanceData
{
    uint ObjectID;
    uint BatchIndex;
    float2 Padding;
};

struct Vertex
{
    float4 Position;
    float4 Normal;
    float2 TexCoord;
    float4 Color;
    float4 Tangent;
};

struct DrawIndirectCommand
{
    uint VertexCountPerInstance;
    uint InstanceCount;
    uint StartVertexLocation;
    uint StartInstanceLocation;
};

struct DrawIndexedIndirectCommand
{
    uint NumIndices;
    uint NumInstances;
    uint FirstIndex;
    int VertexOffset;
    uint FirstInstance;
};

struct DrawArguments
{
    uint MeshID;
    uint MaterialID;
    uint InstanceOffset;
    uint InstanceCount;
};

#define MAX_TEXTURE_COUNT 1024

Texture2D g_Textures[MAX_TEXTURE_COUNT] : register(t0, space0); // Bindless needs to be in space0 for Metal

ConstantBuffer<GPUGlobalData> g_GlobalData : register(b0, space1);

StructuredBuffer<GPUObjectData> g_ObjectBuffer : register(t0, space1);
StructuredBuffer<GPUMaterialData> g_MaterialBuffer : register(t1, space1);
StructuredBuffer<GPUMeshData> g_MeshBuffer : register(t2, space1);
StructuredBuffer<GPUInstanceData> g_InstanceBuffer : register(t3, space1);
StructuredBuffer<Vertex> g_VertexBuffer : register(t4, space1);
StructuredBuffer<uint> g_IndexBuffer : register(t5, space1);
StructuredBuffer<DrawArguments> g_DrawArgsBuffer : register(t6, space1);

SamplerState g_LinearSampler : register(s0, space2);
SamplerState g_PointSampler : register(s1, space2);
SamplerState g_AnisotropicSampler : register(s2, space2);