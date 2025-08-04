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

#include "DZEngine/Utilities/MathConverter.h"

#include <glm/detail/type_quat.hpp>

using namespace DZEngine;

glm::vec3 MathConverter::ToGlm( const DenOfIz::Float3 &interopVec3 )
{
    return glm::vec3( interopVec3.X, interopVec3.Y, interopVec3.Z );
}

DenOfIz::Float3 MathConverter::ToInterop( const glm::vec3 &glmVec3 )
{
    return DenOfIz::Float3{ glmVec3.x, glmVec3.y, glmVec3.z };
}

glm::vec4 MathConverter::ToGlm( const DenOfIz::Float4 &float4 )
{
    return glm::vec4( float4.X, float4.Y, float4.Z, float4.W );
}

glm::quat MathConverter::ToGlmQuat( const DenOfIz::Quaternion &float4 )
{
    return glm::quat( float4.W, float4.X, float4.Y, float4.Z );
}

DenOfIz::Float4 MathConverter::ToInterop( const glm::vec4 &glmVec4 )
{
    return DenOfIz::Float4{ glmVec4.x, glmVec4.y, glmVec4.z, glmVec4.w };
}

glm::mat4 MathConverter::ToGlm( const DenOfIz::Float4x4 &interopMat4 )
{
    return glm::mat4( interopMat4._11, interopMat4._21, interopMat4._31, interopMat4._41, interopMat4._12, interopMat4._22, interopMat4._32, interopMat4._42, interopMat4._13,
                      interopMat4._23, interopMat4._33, interopMat4._43, interopMat4._14, interopMat4._24, interopMat4._34, interopMat4._44 );
}

DenOfIz::Float4x4 MathConverter::ToInterop( const glm::mat4 &glmMat4 )
{
    return DenOfIz::Float4x4{ glmMat4[ 0 ][ 0 ], glmMat4[ 0 ][ 1 ], glmMat4[ 0 ][ 2 ], glmMat4[ 0 ][ 3 ], glmMat4[ 1 ][ 0 ], glmMat4[ 1 ][ 1 ],
                              glmMat4[ 1 ][ 2 ], glmMat4[ 1 ][ 3 ], glmMat4[ 2 ][ 0 ], glmMat4[ 2 ][ 1 ], glmMat4[ 2 ][ 2 ], glmMat4[ 2 ][ 3 ],
                              glmMat4[ 3 ][ 0 ], glmMat4[ 3 ][ 1 ], glmMat4[ 3 ][ 2 ], glmMat4[ 3 ][ 3 ] };
}

DenOfIz::Quaternion MathConverter::ToInterop( const glm::quat &glmQuat )
{
    return DenOfIz::Quaternion{ glmQuat.x, glmQuat.y, glmQuat.z, glmQuat.w };
}
