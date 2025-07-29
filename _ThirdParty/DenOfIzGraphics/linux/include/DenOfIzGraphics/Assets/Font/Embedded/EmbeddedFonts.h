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

#include <mutex>
#include <string>
#include <unordered_map>
#include "DenOfIzGraphics/Assets/Serde/Font/FontAsset.h"
#include "DenOfIzGraphics/Utilities/Interop.h"

namespace DenOfIz
{
    class EmbeddedFonts
    {
    public:
        DZ_API static FontAsset *GetEmbeddedFont( const std::string &fontName );

        DZ_API static FontAsset *GetJetbrainsMono( )
        {
            return GetEmbeddedFont( "JetbrainsMono" );
        }

        DZ_API static FontAsset *GetInter( )
        {
            return GetEmbeddedFont( "Inter" );
        }
    private:
        struct FontData
        {
            const uint8_t *compressedData;
            size_t         compressedSize;
        };

        struct DecompressedFontData
        {
            std::vector<Byte> data;
            bool              isDecompressed = false;
        };

        static std::unordered_map<std::string, FontData>             &GetFontRegistry( );
        static std::unordered_map<std::string, DecompressedFontData> &GetDecompressedFontCache( );
        static std::mutex                                            &GetDecompressionMutex( );
        static const std::vector<Byte>                               &GetFontData( const std::string &fontName );
        static FontAsset                                             *CreateFontAsset( const std::string &fontName );
    };
} // namespace DenOfIz
