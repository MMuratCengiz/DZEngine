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

#include "BindingSlotMap.h"
#include "DenOfIzGraphics/Backends/Interface/IResourceBindGroup.h"
#include "DenOfIzGraphics/Support/GPUBufferView.h"

namespace DenOfIz
{
    struct DZ_API BindingUpdateContextDesc
    {
        IResourceBindGroup *BindGroup;
        BindingSlotMap     *BindingTable;
        BindingFrequency    Frequency;
        GPUBufferView       WriteBuffer;
        Byte               *WriteLocation;
        
        // Null resources for automatic binding
        ITextureResource   *NullTexture = nullptr;
        ISampler           *NullSampler = nullptr;
        GPUBufferView       NullBuffer;
    };

    class BindingUpdateContext
    {
        enum class SlotValueType
        {
            Texture,
            TextureArray,
            Sampler,
            Buffer,
            Data
        };

        struct SlotValue
        {
            union
            {
                ITextureResource     *Texture;
                TextureResourceArray *TextureArray;
                ISampler             *Sampler;
                GPUBufferView         Buffer;
                ByteArrayView         Data;
            } Value;
            SlotValueType Type;
        };

        std::unordered_map<std::string, SlotValue> m_currentBindings;

        GPUBufferView       m_writeBuffer;
        Byte               *m_writeLocation;
        BindingFrequency    m_frequency;
        IResourceBindGroup *m_bindGroup;
        BindingSlotMap     *m_slotTable;
        bool                m_isDirty;
        
        // Null resources for automatic binding
        ITextureResource   *m_nullTexture;
        ISampler           *m_nullSampler;
        GPUBufferView       m_nullBuffer;

    public:
        DZ_API explicit BindingUpdateContext( const BindingUpdateContextDesc &desc );
        DZ_API ~BindingUpdateContext( );

        DZ_API void                SetData( const StringView &name, const ByteArrayView &data );
        DZ_API void                SetBuffer( const StringView &name, const GPUBufferView &bufferView );
        DZ_API void                SetTexture( const StringView &name, ITextureResource *texture );
        DZ_API void                SetTextureArray( const StringView &name, TextureResourceArray *textureResourceArray );
        DZ_API void                SetSampler( const StringView &name, ISampler *sampler );
        DZ_API IResourceBindGroup *GetBindGroup( );

    private:
        void BindCbv( const SlotValue &slotValue, const FrequencyBindingSlot &slot ) const;
        void BindSrv( const SlotValue &slotValue, const FrequencyBindingSlot &slot ) const;
        void BindUav( const SlotValue &slotValue, const FrequencyBindingSlot &slot ) const;
        void BindSampler( const SlotValue &slotValue, const FrequencyBindingSlot &slot ) const;
        void BindNullResource( const FrequencyBindingSlot &slot ) const;

        SlotValue *GetSlotValue( const StringView &name, const SlotValueType &type );
        void       UpdateSlot( const StringView &name, const SlotValue &value );
    };
} // namespace DenOfIz
