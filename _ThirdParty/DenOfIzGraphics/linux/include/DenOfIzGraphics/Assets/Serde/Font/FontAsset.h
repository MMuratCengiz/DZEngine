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

#include "DenOfIzGraphics/Assets/Serde/Asset.h"
#include "DenOfIzGraphics/Utilities/Common_Arrays.h"
#include "DenOfIzGraphics/Utilities/DZArena.h"

namespace DenOfIz
{
    enum class AntiAliasingMode
    {
        None,
        Grayscale,
        Subpixel
    };
    struct DZ_API GlyphBounds
    {
        double XMin = 0;
        double YMin = 0;
        double XMax = 0;
        double YMax = 0;
    };
    struct DZ_API FontGlyph
    {
        uint32_t    CodePoint = 0;
        GlyphBounds Bounds    = { };
        uint32_t    Width     = 0;
        uint32_t    Height    = 0;
        float       BearingX  = 0.0f;
        float       BearingY  = 0.0f;
        float       XAdvance  = 0.0f;
        float       YAdvance  = 0.0f;
        uint32_t    AtlasX    = 0;
        uint32_t    AtlasY    = 0;
    };

    struct DZ_API FontGlyphArray
    {
        FontGlyph *Elements;
        size_t     NumElements;
    };

    struct DZ_API FontMetrics
    {
        float Ascent;
        float Descent;
        float LineGap;
        float LineHeight;
        float UnderlinePos;
        float UnderlineThickness;
    };

    struct DZ_API FontAsset : public AssetHeader
    {
        DZArena _Arena{ sizeof( FontAsset ) };

        /*
         * Version 2: Changed metrics to float
         */
        static constexpr uint32_t Latest      = 2;
        static constexpr uint32_t NumChannels = 4;

        uint64_t          DataNumBytes = 0;
        ByteArray         Data;
        uint32_t          InitialFontSize;
        AntiAliasingMode  AntiAliasingMode;
        uint32_t          AtlasWidth;
        uint32_t          AtlasHeight;
        FontMetrics       Metrics;
        FontGlyphArray    Glyphs;
        UserPropertyArray UserProperties;
        uint64_t          NumAtlasDataBytes = 0;
        ByteArray         AtlasData; // RGBA (MTSDF format)

        FontAsset( ) : AssetHeader( 0x544E4F465A44 /*DZFONT*/, Latest, 0 )
        {
            InitialFontSize = 36;
            AtlasWidth      = 512;
            AtlasHeight     = 512;

            Metrics.Ascent             = 0;
            Metrics.Descent            = 0;
            Metrics.LineGap            = 0;
            Metrics.LineHeight         = 0;
            Metrics.UnderlinePos       = 0;
            Metrics.UnderlineThickness = 0;
        }

        static InteropString Extension( )
        {
            return "dzfont";
        }
    };
} // namespace DenOfIz
