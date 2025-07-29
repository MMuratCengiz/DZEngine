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

#include "DenOfIzGraphics/Backends/Interface/IRootSignature.h"
#include "DenOfIzGraphics/Utilities/Common_Arrays.h"
#include "ShaderBindingTypes.h"

#include <unordered_map>

namespace DenOfIz
{
    struct DZ_API BindingSlotMapDesc
    {
        RootSignatureDesc RootSignatureDesc;
    };

    struct DZ_API BindingSlotDataRange
    {
        size_t NumBytes;
        size_t Offset;
    };

    class DZ_API BindingSlotMap
    {
        struct FrequencyData
        {
            uint64_t                 RequiredBufferBytes = 0;
            std::vector<std::string> Bindings;
            std::vector<StringView>  BindingNameViews;
        };

        std::vector<FrequencyData>                            m_frequencyData; // Index is binding frequency to int
        std::unordered_map<std::string, FrequencyBindingSlot> m_resourceSlots;

    public:
        explicit BindingSlotMap( const BindingSlotMapDesc &desc );
        StringArray                               GetFrequencyBindingNames( BindingFrequency frequency );
        [[nodiscard]] const FrequencyBindingSlot &GetBindingSlot( const StringView &name ) const;
        [[nodiscard]] size_t                      GetFrequencyRequiredDataBytes( const BindingFrequency &frequency ) const;

    private:
        FrequencyData &GetFrequencyData( BindingFrequency &frequency );
    };
} // namespace DenOfIz
