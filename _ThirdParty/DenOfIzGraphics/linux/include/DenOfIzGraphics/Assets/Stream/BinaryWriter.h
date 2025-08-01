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

#include <ostream>
#include "BinaryContainer.h"
#include "DenOfIzGraphics/Utilities/Common_Arrays.h"
#include "DenOfIzGraphics/Utilities/InteropMath.h"

namespace DenOfIz
{
    struct DZ_API BinaryWriterDesc{
        // None for now
    };

    class BinaryWriter
    {
        BinaryWriterDesc m_desc;
        bool             m_isStreamOwned = false;
        bool             m_isStreamValid;
        std::ostream    *m_stream;

    public:
        // Non public api since std::ostream cannot be mapped
        explicit BinaryWriter( std::ostream *stream, const BinaryWriterDesc &desc = { } );
        DZ_API explicit BinaryWriter( BinaryContainer &container, const BinaryWriterDesc &desc = { } );
        DZ_API explicit BinaryWriter( const InteropString &filePath, const BinaryWriterDesc &desc = { } );
        DZ_API ~BinaryWriter( );

        DZ_API void                   WriteByte( Byte value ) const;
        DZ_API void                   Write( const ByteArrayView &buffer, uint32_t offset, uint32_t count ) const;
        DZ_API void                   WriteBytes( const ByteArrayView &buffer ) const;
        DZ_API void                   WriteUInt16( uint16_t value ) const;
        DZ_API void                   WriteUInt32( uint32_t value ) const;
        DZ_API void                   WriteUInt64( uint64_t value ) const;
        DZ_API void                   WriteInt16( int16_t value ) const;
        DZ_API void                   WriteInt32( int32_t value ) const;
        DZ_API void                   WriteInt64( int64_t value ) const;
        DZ_API void                   WriteFloat( float value ) const;
        DZ_API void                   WriteDouble( double value ) const;
        DZ_API void                   WriteString( const InteropString &value ) const;
        DZ_API void                   WriteUInt16_2( const UShort2 &value ) const;
        DZ_API void                   WriteUInt16_3( const UShort3 &value ) const;
        DZ_API void                   WriteUInt16_4( const UShort4 &value ) const;
        DZ_API void                   WriteInt16_2( const Short2 &value ) const;
        DZ_API void                   WriteInt16_3( const Short3 &value ) const;
        DZ_API void                   WriteInt16_4( const Short4 &value ) const;
        DZ_API void                   WriteUInt32_2( const UInt2 &value ) const;
        DZ_API void                   WriteUInt32_3( const UInt3 &value ) const;
        DZ_API void                   WriteUInt32_4( const UInt4 &value ) const;
        DZ_API void                   WriteInt32_2( const Int2 &value ) const;
        DZ_API void                   WriteInt32_3( const Int3 &value ) const;
        DZ_API void                   WriteInt32_4( const Int4 &value ) const;
        DZ_API void                   WriteFloat_2( const Float2 &value ) const;
        DZ_API void                   WriteFloat_3( const Float3 &value ) const;
        DZ_API void                   WriteFloat_4( const Float4 &value ) const;
        DZ_API void                   WriteFloat_4x4( const Float4x4 &value ) const;
        DZ_API [[nodiscard]] uint64_t Position( ) const;
        DZ_API void                   Seek( uint64_t position ) const;
        DZ_API void                   Flush( ) const;
    };
} // namespace DenOfIz
