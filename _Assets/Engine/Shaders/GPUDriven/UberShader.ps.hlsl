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

struct PSOutput
{
    float4 Color : SV_TARGET0;
};

float3 GetNormalFromMap(float3 normal, float4 tangent, float2 uv, uint normalTexIdx)
{
    if (normalTexIdx >= MAX_TEXTURE_COUNT)
    {
        return normal;
    }
    
    float3 tangentNormal = g_Textures[normalTexIdx].Sample(g_LinearSampler, uv).xyz * 2.0 - 1.0;
    
    float3 N = normalize(normal);
    float3 T = normalize(tangent.xyz);
    float3 B = normalize(cross(N, T)) * tangent.w;
    float3x3 TBN = float3x3(T, B, N);
    
    return normalize(mul(tangentNormal, TBN));
}

PSOutput PSMain(VSOutput input)
{
    GPUMaterialData material = g_MaterialBuffer[input.MaterialID];
    
    float4 baseColor = material.BaseColorFactor * input.Color;
    if (material.BaseColorTexture < MAX_TEXTURE_COUNT)
    {
        baseColor *= g_Textures[material.BaseColorTexture].Sample(g_LinearSampler, input.TexCoord);
    }
    
    float3 normal = input.Normal;
    if (material.NormalTexture < MAX_TEXTURE_COUNT)
    {
        normal = GetNormalFromMap(input.Normal, input.Tangent, input.TexCoord, material.NormalTexture);
    }
    
    float metallic = material.MetallicFactor;
    float roughness = material.RoughnessFactor;
    if (material.MetallicRoughnessTexture < MAX_TEXTURE_COUNT)
    {
        float4 metallicRoughness = g_Textures[material.MetallicRoughnessTexture].Sample(g_LinearSampler, input.TexCoord);
        metallic *= metallicRoughness.b;
        roughness *= metallicRoughness.g;
    }
    
    float occlusion = 1.0;
    if (material.OcclusionTexture < MAX_TEXTURE_COUNT)
    {
        occlusion = g_Textures[material.OcclusionTexture].Sample(g_LinearSampler, input.TexCoord).r;
        occlusion = lerp(1.0, occlusion, material.OcclusionStrength);
    }
    
    float3 emissive = material.EmissiveFactor.rgb;
    if (material.EmissiveTexture < MAX_TEXTURE_COUNT)
    {
        emissive *= g_Textures[material.EmissiveTexture].Sample(g_LinearSampler, input.TexCoord).rgb;
    }
    
    float3 V = normalize(g_GlobalData.CameraPosition.xyz - input.WorldPos);
    float3 L = normalize(float3(1, 1, 1));
    float3 H = normalize(V + L);
    float NdotL = max(dot(normal, L), 0.0);
    float NdotV = max(dot(normal, V), 0.0);
    float NdotH = max(dot(normal, H), 0.0);
    float VdotH = max(dot(V, H), 0.0);
    
    float D = ((roughness * roughness) / (3.14159265 * pow(NdotH * NdotH * (roughness * roughness - 1.0) + 1.0, 2.0)));
    float G_V = NdotV / (NdotV * (1.0 - roughness * 0.5) + roughness * 0.5);
    float G_L = NdotL / (NdotL * (1.0 - roughness * 0.5) + roughness * 0.5);
    float G = G_V * G_L;
    float3 F0 = lerp(float3(0.04, 0.04, 0.04), baseColor.rgb, metallic);
    float3 F = F0 + (1.0 - F0) * pow(1.0 - VdotH, 5.0);
    
    float3 specular = D * G * F / (4.0 * NdotV * NdotL + 0.001);
    float3 diffuse = baseColor.rgb * (1.0 - metallic) / 3.14159265;
    
    float3 color = emissive + (diffuse + specular) * NdotL * occlusion;
    
    PSOutput output;
    output.Color = float4(color, baseColor.a);
    return output;
}