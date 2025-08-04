#include "DZEngine/Rendering/GPUDriven/GPUDrivenRootSig.h"
#include <spdlog/spdlog.h>
#include <vector>
#include "DZEngine/Rendering/GPUDriven/GPUDrivenSceneData.h"
#include "DenOfIzGraphics/Support/ShaderBinding/ShaderBindingTypes.h"

using namespace DZEngine;
using namespace DenOfIz;

GPUDrivenRootSig::GPUDrivenRootSig( ILogicalDevice *device )
{
    std::vector allStages = { ShaderStage::Vertex, ShaderStage::Pixel, ShaderStage::Compute };

    ResourceBindingDesc globalDataBinding{ };
    globalDataBinding.Name               = "g_GlobalData";
    globalDataBinding.DataType           = BindingDataType::Struct;
    globalDataBinding.NumBytes           = sizeof( GPUGlobalData );
    globalDataBinding.Descriptor         = ResourceDescriptor::UniformBuffer;
    globalDataBinding.BindingType        = ResourceBindingType::ConstantBuffer;
    globalDataBinding.Binding            = 0;
    globalDataBinding.RegisterSpace      = 1;
    globalDataBinding.ArraySize          = 1;
    globalDataBinding.Stages.Elements    = allStages.data( );
    globalDataBinding.Stages.NumElements = allStages.size( );
    m_resourceBindings.push_back( globalDataBinding );

    ResourceBindingDesc objectBufferBinding{ };
    objectBufferBinding.Name          = "g_ObjectBuffer";
    objectBufferBinding.DataType      = BindingDataType::Struct;
    objectBufferBinding.NumBytes      = sizeof( GPUObjectData );
    objectBufferBinding.Descriptor    = ResourceDescriptor::StructuredBuffer;
    objectBufferBinding.BindingType   = ResourceBindingType::ShaderResource;
    objectBufferBinding.Binding       = 0;
    objectBufferBinding.RegisterSpace = 1;
    objectBufferBinding.ArraySize     = 1;
    objectBufferBinding.Stages        = globalDataBinding.Stages;
    m_resourceBindings.push_back( objectBufferBinding );

    ResourceBindingDesc materialBufferBinding{ };
    materialBufferBinding.Name          = "g_MaterialBuffer";
    materialBufferBinding.DataType      = BindingDataType::Struct;
    materialBufferBinding.NumBytes      = sizeof( GPUMaterialData );
    materialBufferBinding.Descriptor    = ResourceDescriptor::StructuredBuffer;
    materialBufferBinding.BindingType   = ResourceBindingType::ShaderResource;
    materialBufferBinding.Binding       = 1;
    materialBufferBinding.RegisterSpace = 1;
    materialBufferBinding.ArraySize     = 1;
    materialBufferBinding.Stages        = globalDataBinding.Stages;
    m_resourceBindings.push_back( materialBufferBinding );

    ResourceBindingDesc meshBufferBinding{ };
    meshBufferBinding.Name          = "g_MeshBuffer";
    meshBufferBinding.DataType      = BindingDataType::Struct;
    meshBufferBinding.NumBytes      = sizeof( GPUMeshData );
    meshBufferBinding.Descriptor    = ResourceDescriptor::StructuredBuffer;
    meshBufferBinding.BindingType   = ResourceBindingType::ShaderResource;
    meshBufferBinding.Binding       = 2;
    meshBufferBinding.RegisterSpace = 1;
    meshBufferBinding.ArraySize     = 1;
    meshBufferBinding.Stages        = globalDataBinding.Stages;
    m_resourceBindings.push_back( meshBufferBinding );

    ResourceBindingDesc instanceBufferBinding{ };
    instanceBufferBinding.Name          = "g_InstanceBuffer";
    instanceBufferBinding.DataType      = BindingDataType::Struct;
    instanceBufferBinding.NumBytes      = sizeof( GPUInstanceData );
    instanceBufferBinding.Descriptor    = ResourceDescriptor::UniformBuffer;
    instanceBufferBinding.BindingType   = ResourceBindingType::ConstantBuffer;
    instanceBufferBinding.Binding       = 1;
    instanceBufferBinding.RegisterSpace = 1;
    instanceBufferBinding.ArraySize     = 1;
    instanceBufferBinding.Stages        = globalDataBinding.Stages;
    m_resourceBindings.push_back( instanceBufferBinding );

    ResourceBindingDesc textureArrayBinding{ };
    textureArrayBinding.Name          = "g_Textures";
    textureArrayBinding.DataType      = BindingDataType::Texture;
    textureArrayBinding.Descriptor    = ResourceDescriptor::Texture;
    textureArrayBinding.BindingType   = ResourceBindingType::ShaderResource;
    textureArrayBinding.Binding       = 0;
    textureArrayBinding.RegisterSpace = 0;
    textureArrayBinding.ArraySize     = MaxNumTextures;
    textureArrayBinding.Stages        = globalDataBinding.Stages;
    m_resourceBindings.push_back( textureArrayBinding );

    ResourceBindingDesc linearSamplerBinding{ };
    linearSamplerBinding.Name          = "g_LinearSampler";
    linearSamplerBinding.DataType      = BindingDataType::SamplerDesc;
    linearSamplerBinding.Descriptor    = ResourceDescriptor::Sampler;
    linearSamplerBinding.BindingType   = ResourceBindingType::Sampler;
    linearSamplerBinding.Binding       = 0;
    linearSamplerBinding.RegisterSpace = 2;
    linearSamplerBinding.ArraySize     = 1;
    linearSamplerBinding.Stages        = globalDataBinding.Stages;
    m_resourceBindings.push_back( linearSamplerBinding );

    ResourceBindingDesc pointSamplerBinding{ };
    pointSamplerBinding.Name          = "g_PointSampler";
    pointSamplerBinding.DataType      = BindingDataType::SamplerDesc;
    pointSamplerBinding.Descriptor    = ResourceDescriptor::Sampler;
    pointSamplerBinding.BindingType   = ResourceBindingType::Sampler;
    pointSamplerBinding.Binding       = 1;
    pointSamplerBinding.RegisterSpace = 2;
    pointSamplerBinding.ArraySize     = 1;
    pointSamplerBinding.Stages        = globalDataBinding.Stages;
    m_resourceBindings.push_back( pointSamplerBinding );

    ResourceBindingDesc anisotropicSamplerBinding{ };
    anisotropicSamplerBinding.Name          = "g_AnisotropicSampler";
    anisotropicSamplerBinding.DataType      = BindingDataType::SamplerDesc;
    anisotropicSamplerBinding.Descriptor    = ResourceDescriptor::Sampler;
    anisotropicSamplerBinding.BindingType   = ResourceBindingType::Sampler;
    anisotropicSamplerBinding.Binding       = 2;
    anisotropicSamplerBinding.RegisterSpace = 2;
    anisotropicSamplerBinding.ArraySize     = 1;
    anisotropicSamplerBinding.Stages        = globalDataBinding.Stages;
    m_resourceBindings.push_back( anisotropicSamplerBinding );

    m_desc.ResourceBindings.Elements    = m_resourceBindings.data( );
    m_desc.ResourceBindings.NumElements = static_cast<uint32_t>( m_resourceBindings.size( ) );

    m_rootSignature.reset( device->CreateRootSignature( m_desc ) );

    if ( !m_rootSignature )
    {
        spdlog::error( "Failed to create GPUDriven root signature" );
    }
}

RootSignatureDesc GPUDrivenRootSig::GetDesc( ) const
{
    return m_desc;
}

IRootSignature *GPUDrivenRootSig::GetRootSignature( ) const
{
    return m_rootSignature.get( );
}
