#include "GPUDrivenRootSignature.hlsli"

struct VSOutput
{
    float4 Position : SV_POSITION;
    float3 WorldPos : WORLD_POS;
    float3 Normal : NORMAL;
    float4 Tangent : TANGENT;
    float2 TexCoord : TEXCOORD0;
    float4 Color : COLOR0;
    uint MaterialID : MATERIAL_ID;
    uint ObjectID : OBJECT_ID;
    uint MeshID : MESH_ID;
};

VSOutput VSMain(uint vertexID : SV_VertexID, uint instanceID : SV_InstanceID)
{
    uint instanceIndex = instanceID;
    GPUInstanceData instanceData = g_InstanceBuffer[instanceIndex];
    
    GPUObjectData objectData = g_ObjectBuffer[instanceData.ObjectID];
    GPUMeshData meshData = g_MeshBuffer[objectData.MeshID];
    
    Vertex vertex = g_VertexBuffer[meshData.VertexOffset + vertexID];
    
    float4 worldPos = mul(vertex.Position, objectData.ModelMatrix);
    float4 clipPos = mul(worldPos, g_GlobalData.ViewProjMatrix);
    
    float3x3 normalMatrix = (float3x3)objectData.ModelMatrix;
    float3 worldNormal = normalize(mul(vertex.Normal.xyz, normalMatrix));
    float4 worldTangent = float4(normalize(mul(vertex.Tangent.xyz, normalMatrix)), vertex.Tangent.w);
    
    VSOutput output;
    output.Position = clipPos;
    output.WorldPos = worldPos.xyz;
    output.Normal = worldNormal;
    output.Tangent = worldTangent;
    output.TexCoord = vertex.TexCoord;
    output.Color = vertex.Color;
    output.MaterialID = objectData.MaterialID;
    output.ObjectID = instanceData.ObjectID;
    output.MeshID = objectData.MeshID;
    
    return output;
}