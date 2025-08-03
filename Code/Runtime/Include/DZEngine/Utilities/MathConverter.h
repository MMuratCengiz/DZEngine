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
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace DZEngine
{
    class MathConverter
    {
    public:
        static glm::vec3           ToGlm( const DenOfIz::Float3 &interopVec3 );
        static DenOfIz::Float3     ToInterop( const glm::vec3 &glmVec3 );
        static glm::vec4           ToGlm( const DenOfIz::Float4 &float4 );
        static DenOfIz::Float4     ToInterop( const glm::vec4 &glmVec4 );
        static glm::mat4           ToGlm( const DenOfIz::Float4x4 &interopMat4 );
        static DenOfIz::Float4x4   ToInterop( const glm::mat4 &glmMat4 );
        static DenOfIz::Quaternion ToInterop( const glm::quat &glmQuat );
    };
} // namespace DZEngine
