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

#include "DenOfIzGraphics/Assets/Stream/BinaryReader.h"
#include "PhysicsAsset.h"

namespace DenOfIz
{
    struct DZ_API PhysicsAssetReaderDesc
    {
        BinaryReader *Reader;
    };

    class PhysicsAssetReader
    {
        BinaryReader *m_reader;
        PhysicsAsset *m_physicsAsset;

    public:
        DZ_API explicit PhysicsAssetReader( const PhysicsAssetReaderDesc &desc );
        DZ_API ~PhysicsAssetReader( );

        DZ_API PhysicsAsset *Read( );
    };
} // namespace DenOfIz
