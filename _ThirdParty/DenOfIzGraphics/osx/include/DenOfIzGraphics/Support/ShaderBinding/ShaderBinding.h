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

#include <unordered_map>
#include "BindingSlotMap.h"
#include "BindingUpdateContext.h"

#include "DenOfIzGraphics/Backends/Interface/ILogicalDevice.h"
#include "DenOfIzGraphics/Support/RingBuffer.h"

namespace DenOfIz
{

    enum class DrawIndexingMethod
    {
        ObjectId,
        RoundRobin
    };

    struct DZ_API ShaderBindingDesc
    {
        ILogicalDevice    *LogicalDevice = nullptr;
        RootSignatureDesc  RootSignatureDesc; // Required for the layout, so we can map names to slots
        IRootSignature    *RootSignature          = nullptr;
        DrawIndexingMethod DrawIndexingMethod     = DrawIndexingMethod::ObjectId;
        uint32_t           NumGlobalThreads       = 1;
        uint32_t           NumViewThreads         = 3;
        uint32_t           NumMaterialThreads     = 3;
        uint32_t           NumDrawThreads         = 3;
        uint32_t           NumMaxGlobalElements   = 1;
        uint32_t           NumMaxViewElements     = 16;
        uint32_t           NumMaxMaterialElements = 128;
        uint32_t           NumMaxDrawElements     = 128 * 1024;
    };

    class DZ_API FrequencyBindingSlotTable
    {
    public:
        FrequencyBindingSlot &GetSlot( const StringView &name );
    };

    /*
     * Manages shader bindings for a specific root signature.
     * Automatically handles descriptor set pooling and versioning for frames in flight.
     *
     * Usage:
     *   // BindingFrequency::Once
     *   shaderBinding.SetGlobalTextureArray( "GlobalTextureName", textureArray );
     *   shaderBinding.SetGlobalTexture( "TextureName", texture );
     *   shaderBinding.SetGlobalSampler( "SamplerName", sampler );
     *   shaderBinding.SetGlobalBuffer( "BufferName", buffer );
     *   shaderBinding.SetGlobalData( "DataName", &data );
     *
     *   size_t newView = shaderBinding.NewView( );
     *   shaderBinding.SetViewTexture( newView, texture );
     *
     *   shaderBinding.SetFloat( "_Time", 1.0f );
     *   shaderBinding.SetTexture( "_MainTex", texture );
     *   shaderBinding.SetTextureArray( "_AllTextures", textureArray ); // Bindless
     *
     *   // Get bind groups - returns versioned descriptors
     *   cmd->BindResourceGroup( shaderBinding.GetBindGroup( BindingFrequency::Frame ) );
     *   cmd->BindResourceGroup( shaderBinding.GetBindGroup( BindingFrequency::Material ) );
     *
     *   // Notify when frame is submitted
     *   shaderBinding.OnFrameSubmitted();
     */
    class ShaderBinding
    {
        ILogicalDevice                 *m_logicalDevice;
        IRootSignature                 *m_rootSignature;
        std::unique_ptr<BindingSlotMap> m_bindingTable;
        
        // Shared null resources for automatic binding
        std::unique_ptr<ITextureResource> m_nullTexture;
        std::unique_ptr<ISampler>         m_nullSampler;
        std::unique_ptr<IBufferResource>  m_nullBufferResource;
        GPUBufferView                     m_nullBuffer;

        struct FrequencyThreadData : NonCopyable
        {
            std::vector<std::unique_ptr<IResourceBindGroup>>   BindGroups;
            std::unique_ptr<RingBuffer>                        DataBuffer;
            std::vector<std::unique_ptr<BindingUpdateContext>> Contexts;
        };
        struct FrequencyData : NonCopyable
        {
            size_t                                            NumMaxElements = 0;
            size_t                                            CurrentId      = 0;
            std::unordered_map<std::string, size_t>           IndexAliases;
            std::vector<std::unique_ptr<FrequencyThreadData>> ThreadData;
        };
        std::vector<std::unique_ptr<FrequencyData>> m_frequencyData; // Index is static_cast<uint32_t>( BindingFrequency );

    public:
        DZ_API explicit ShaderBinding( const ShaderBindingDesc &desc );
        DZ_API ~ShaderBinding( );

        DZ_API size_t NewGlobal( StringView alias = { } ) const;
        DZ_API size_t NewView( StringView alias = { } ) const;
        DZ_API size_t NewMaterial( StringView alias = { } ) const;
        DZ_API size_t NewDraw( StringView alias = { } ) const;

        DZ_API size_t NumGlobals( ) const;
        DZ_API size_t NumViews( ) const;
        DZ_API size_t NumMaterials( ) const;
        DZ_API size_t NumDraws( ) const;

        // if requiredId = 0, Grow functions will add an additional element
        DZ_API size_t GrowGlobals( size_t requiredId = 0 ) const;
        DZ_API size_t GrowViews( size_t requiredId = 0 ) const;
        DZ_API size_t GrowMaterials( size_t requiredId = 0 ) const;
        DZ_API size_t GrowDraws( size_t requiredId = 0 ) const;

        DZ_API BindingUpdateContext *GlobalContext( size_t globalId, size_t threadId ) const;
        DZ_API BindingUpdateContext *ViewContext( size_t view, size_t threadId ) const;
        DZ_API BindingUpdateContext *MaterialContext( size_t materialId, size_t threadId ) const;
        DZ_API BindingUpdateContext *DrawContext( size_t drawId, size_t threadId ) const;

        DZ_API BindingUpdateContext *GlobalContext( StringView alias, size_t threadId ) const;
        DZ_API BindingUpdateContext *ViewContext( StringView alias, size_t threadId ) const;
        DZ_API BindingUpdateContext *MaterialContext( StringView alias, size_t threadId ) const;
        DZ_API BindingUpdateContext *DrawContext( StringView alias, size_t threadId ) const;

    private:
        void InitializeNullResources( );
        void                  InitializeFrequencyData( BindingFrequency frequency, const uint32_t &numThreads, const uint32_t &numElements ) const;
        size_t                NewFrequencyElement( BindingFrequency frequency, StringView alias ) const;
        size_t                GetAliasIndex( BindingFrequency frequency, StringView alias, std::string frequencyName ) const;
        BindingUpdateContext *GetFrequencyContext( BindingFrequency frequency, size_t threadId, size_t elementIndex ) const;
    };
} // namespace DenOfIz
