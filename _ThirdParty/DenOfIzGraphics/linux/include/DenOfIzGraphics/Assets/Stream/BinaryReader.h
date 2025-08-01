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

#include <istream>
#include <vector>
#include "BinaryContainer.h"
#include "DenOfIzGraphics/Utilities/Common_Arrays.h"
#include "DenOfIzGraphics/Utilities/Interop.h"
#include "DenOfIzGraphics/Utilities/InteropMath.h"

namespace DenOfIz
{
    struct DZ_API BinaryReaderDesc
    {
        uint64_t NumBytes = 0; // 0, reading more than this amount of bytes won't be allowed
    };

    class BinaryReader
    {
        uint64_t                               m_allowedNumBytes;
        uint64_t                               m_readNumBytes  = 0;
        bool                                   m_isStreamOwned = false;
        bool                                   m_isStreamValid;
        bool                                   m_isStringStream = false;
        std::istream                          *m_stream;
        mutable std::vector<std::vector<char>> m_stringStorage;

    public:
        explicit BinaryReader( std::istream *stream, const BinaryReaderDesc &desc = { } );
        DZ_API explicit BinaryReader( BinaryContainer &container, const BinaryReaderDesc &desc = { } );
        DZ_API explicit BinaryReader( const InteropString &filePath, const BinaryReaderDesc &desc = { } );
        DZ_API explicit BinaryReader( const ByteArrayView &data, const BinaryReaderDesc &desc = { } );
        DZ_API ~BinaryReader( );

        [[nodiscard]] DZ_API int         ReadByte( );
        [[nodiscard]] DZ_API int         Read( const ByteArray &buffer, uint32_t offset, uint32_t count );
        [[nodiscard]] DZ_API ByteArray   ReadAllBytes( );
        [[nodiscard]] DZ_API ByteArray   ReadBytes( uint32_t count );
        [[nodiscard]] DZ_API uint16_t    ReadUInt16( );
        [[nodiscard]] DZ_API uint32_t    ReadUInt32( );
        [[nodiscard]] DZ_API uint64_t    ReadUInt64( );
        [[nodiscard]] DZ_API int16_t     ReadInt16( );
        [[nodiscard]] DZ_API int32_t     ReadInt32( );
        [[nodiscard]] DZ_API int64_t     ReadInt64( );
        [[nodiscard]] DZ_API float       ReadFloat( );
        [[nodiscard]] DZ_API double      ReadDouble( );
        [[nodiscard]] DZ_API const char *ReadString( );
        [[nodiscard]] DZ_API UShort2    ReadUInt16_2( );
        [[nodiscard]] DZ_API UShort3    ReadUInt16_3( );
        [[nodiscard]] DZ_API UShort4    ReadUInt16_4( );
        [[nodiscard]] DZ_API Short2     ReadInt16_2( );
        [[nodiscard]] DZ_API Short3     ReadInt16_3( );
        [[nodiscard]] DZ_API Short4     ReadInt16_4( );
        [[nodiscard]] DZ_API UInt2    ReadUInt32_2( );
        [[nodiscard]] DZ_API UInt3    ReadUInt32_3( );
        [[nodiscard]] DZ_API UInt4    ReadUInt32_4( );
        [[nodiscard]] DZ_API Int2     ReadInt32_2( );
        [[nodiscard]] DZ_API Int3     ReadInt32_3( );
        [[nodiscard]] DZ_API Int4     ReadInt32_4( );
        [[nodiscard]] DZ_API Float2     ReadFloat_2( );
        [[nodiscard]] DZ_API Float3     ReadFloat_3( );
        [[nodiscard]] DZ_API Float4     ReadFloat_4( );
        [[nodiscard]] DZ_API Float4x4   ReadFloat_4x4( );
        [[nodiscard]] DZ_API uint64_t    Position( ) const;
        DZ_API void                      Seek( uint64_t position ) const;
        DZ_API void                      Skip( uint64_t count ) const;

        // Utility function to log data as a C++ array for embedding in code
        DZ_API void LogAsCppArray( const InteropString &variableName = "Data" ) const;
        DZ_API void WriteCppArrayToFile( const InteropString &targetFile = "Data.txt" ) const;

    private:
        [[nodiscard]] bool IsStreamValid( ) const;
        bool               TrackReadBytes( uint32_t requested );
    };
} // namespace DenOfIz
