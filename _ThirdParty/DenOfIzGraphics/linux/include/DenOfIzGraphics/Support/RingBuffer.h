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

#include <DenOfIzGraphics/Backends/Interface/IBufferResource.h>
#include <DenOfIzGraphics/Backends/Interface/ILogicalDevice.h>
#include <memory>
#include <vector>
#include "GPUBufferView.h"

using namespace DenOfIz;

namespace DenOfIz
{
    struct RingBufferDesc
    {
        ILogicalDevice *LogicalDevice      = nullptr;
        size_t          DataNumBytes       = 0;
        size_t          NumEntries         = 0;
        size_t          MaxChunkNumBytes   = 65536;
        bool            IsStructuredBuffer = false;
    };

    class RingBuffer : public NonCopyable
    {
        static constexpr size_t ALIGNMENT = 256;

        struct ChunkInfo
        {
            std::unique_ptr<IBufferResource> Buffer;
            Byte                            *MappedMemory = nullptr;
            size_t                           StartIndex;
            size_t                           EndIndex; // Exclusive
            size_t                           NumBytes;
        };

        std::vector<ChunkInfo> m_chunks;
        size_t                 m_dataNumBytes;
        size_t                 m_numEntries;
        size_t                 m_alignedNumBytes;
        size_t                 m_totalNumBytes;
        size_t                 m_maxChunkNumBytes;
        bool                   m_isStructuredBuffer{ };

        bool m_memoryMapped{ };

    public:
        DZ_API explicit RingBuffer( const RingBufferDesc &desc );
        DZ_API ~RingBuffer( );

        DZ_API [[nodiscard]] GPUBufferView    GetBufferView( size_t index ) const;
        DZ_API [[nodiscard]] Byte            *GetMappedMemory( size_t index );
        DZ_API [[nodiscard]] size_t           GetAlignedNumBytes( ) const;
        DZ_API [[nodiscard]] size_t           GetTotalNumBytes( ) const;

    private:
        void MapMemory( );
        static size_t        AlignUp( size_t value, size_t alignment );
        [[nodiscard]] size_t GetChunkIndexForEntry( size_t entryIndex ) const;
    };
} // namespace DenOfIz
