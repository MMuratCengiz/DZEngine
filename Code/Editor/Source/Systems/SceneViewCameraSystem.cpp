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

#include "DZEngine/Systems/SceneViewCameraSystem.h"
#include "DZEngine/Components/CameraComponent.h"
#include "DZEngine/Components/CameraControllerComponent.h"
#include "DZEngine/Components/InputStateComponent.h"
#include "DZEngine/Components/TransformComponent.h"
#include "DZEngine/Utilities/MathConverter.h"

#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace DenOfIz;
using namespace DZEngine;

void SceneViewCameraSystem::Register( const flecs::world &world )
{
    world.system<const InputStateComponent, CameraControllerComponent, TransformComponent, CameraComponent>( )
        .kind( flecs::OnUpdate )
        .each(
            [ = ]( const flecs::iter &it, size_t i, const InputStateComponent &input, CameraControllerComponent &controller, TransformComponent &transform,
                   CameraComponent &camera )
            {
                const float deltaTime = it.delta_time( );

                if ( controller.EnableRotation && input.PressedMouseButtons.contains( MouseButton::Right ) )
                {
                    controller.Yaw += input.MouseDelta.X * controller.LookSpeed;
                    controller.Pitch -= input.MouseDelta.Y * controller.LookSpeed;

                    controller.Pitch = std::clamp( controller.Pitch, controller.MinPitch, controller.MaxPitch );
                }

                if ( controller.EnableMovement )
                {
                    glm::vec3 movement{ 0.0f };
                    auto      forward = glm::vec3( std::cos( controller.Yaw ) * std::cos( controller.Pitch ), std::sin( controller.Pitch ),
                                                   std::sin( controller.Yaw ) * std::cos( controller.Pitch ) );
                    forward           = glm::normalize( forward );

                    const glm::vec3 right = glm::normalize( glm::cross( forward, glm::vec3( 0, 1, 0 ) ) );
                    glm::vec3       up    = glm::normalize( glm::cross( right, forward ) );

                    if ( input.PressedKeys.contains( KeyCode::W ) )
                    {
                        movement += forward;
                    }
                    if ( input.PressedKeys.contains( KeyCode::S ) )
                    {
                        movement -= forward;
                    }
                    if ( input.PressedKeys.contains( KeyCode::A ) )
                    {
                        movement -= right;
                    }
                    if ( input.PressedKeys.contains( KeyCode::D ) )
                    {
                        movement += right;
                    }
                    if ( input.PressedKeys.contains( KeyCode::Q ) )
                    {
                        movement.y -= 1.0f;
                    }
                    if ( input.PressedKeys.contains( KeyCode::E ) )
                    {
                        movement.y += 1.0f;
                    }

                    if ( input.MouseWheel.Y != 0 )
                    {
                        movement += forward * input.MouseWheel.Y * controller.ScrollSpeed;
                    }

                    float moveSpeed = controller.MoveSpeed;
                    if ( input.PressedKeys.contains( KeyCode::LShift ) )
                    {
                        moveSpeed *= 3.0f;
                    }

                    glm::vec3 currentPos = MathConverter::ToGlm( transform.Position );
                    currentPos += movement * moveSpeed * deltaTime;
                    transform.Position = MathConverter::ToInterop( currentPos );
                }

                camera.View     = CreateViewMatrix( transform.Position, controller.Yaw, controller.Pitch );
                camera.Position = Float4{ transform.Position.X, transform.Position.Y, transform.Position.Z, 1.0f };

                const glm::mat4 viewGlm     = MathConverter::ToGlm( camera.View );
                const glm::mat4 projGlm     = MathConverter::ToGlm( camera.Projection );
                const glm::mat4 viewProjGlm = projGlm * viewGlm;

                camera.ViewProjection = MathConverter::ToInterop( viewProjGlm );
            } );
}

Float4x4 SceneViewCameraSystem::CreateViewMatrix( const Float3 &position, const float yaw, const float pitch )
{
    const glm::vec3 pos = MathConverter::ToGlm( position );

    auto front = glm::vec3( std::cos( yaw ) * std::cos( pitch ), std::sin( pitch ), std::sin( yaw ) * std::cos( pitch ) );
    front      = glm::normalize( front );

    constexpr glm::vec3 up( 0.0f, 1.0f, 0.0f );
    const glm::mat4     view = glm::lookAt( pos, pos + front, up );

    return MathConverter::ToInterop( view );
}

Float4x4 SceneViewCameraSystem::CreateProjectionMatrix( const float fov, const float aspectRatio, const float nearPlane, const float farPlane )
{
    const glm::mat4 proj = glm::perspective( fov, aspectRatio, nearPlane, farPlane );
    return MathConverter::ToInterop( proj );
}
