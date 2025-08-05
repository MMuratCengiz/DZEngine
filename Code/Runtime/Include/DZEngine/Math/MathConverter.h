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

#include <DenOfIzGraphics/Utilities/InteropMath.h>
#include "Math.h"

using namespace DenOfIz;

namespace DZEngine
{
    class MathConverter
    {
    public:
        static DirectX::XMFLOAT4X4 Float4x4ToXMFLOAT4X4( const Float4x4 &matrix );
        static Float4x4            Float4x4FromXMFLOAT4X4( const DirectX::XMFLOAT4X4 &matrix );
        static Float4x4            Float4X4FromXMMATRIX( const DirectX::XMMATRIX &matrix );
        static DirectX::XMMATRIX   Float4X4ToXMMATRIX( const Float4x4 &matrix );
        static Float4              Float4FromXMVECTOR( const DirectX::XMVECTOR &vector );
        static Float4              Float4FromXMFLOAT4( const DirectX::XMFLOAT4 &vector );
        static DirectX::XMVECTOR   Float4ToXMVECTOR( const Float4 &vector );
        static Float3              Float3FromXMVECTOR( const DirectX::XMVECTOR &vector );
        static Float3              Float3FromXMFLOAT3( const DirectX::XMFLOAT3 &vector );
        static DirectX::XMVECTOR   Float3ToXMVECTOR( const Float3 &vector );
        static Float2              Float2FromXMVECTOR( const DirectX::XMVECTOR &vector );
        static Float2              Float2FromXMFLOAT2( const DirectX::XMFLOAT2 &vector );
        static DirectX::XMVECTOR   Float2ToXMVECTOR( const Float2 &vector );
    };
} // namespace DZEngine
