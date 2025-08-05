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
#include "DZEngine/Math/Math.h"
#include "DZEngine/Math/MathConverter.h"

#include <cmath>

using namespace DenOfIz;
using namespace DZEngine;
using namespace DirectX;

void SceneViewCameraSystem::Register( const flecs::world &world, const GraphicsWindowHandle *windowHandle )
{
    const auto camera = world.entity( "MainCamera" );
    camera.add<TransformComponent>( );
    camera.add<CameraComponent>( );
    camera.add<CameraControllerComponent>( );

    auto &controller           = camera.get_mut<CameraControllerComponent>( );
    controller.DefaultPosition = { 0.0f, 0.0f, -5.0f };
    controller.DefaultYaw      = -1.5708f;
    controller.DefaultPitch    = 0.0f;
    controller.TargetPosition  = controller.DefaultPosition;
    controller.TargetYaw       = controller.DefaultYaw;
    controller.TargetPitch     = controller.DefaultPitch;
    controller.Yaw             = controller.DefaultYaw;
    controller.Pitch           = controller.DefaultPitch;

    auto &transform    = camera.get_mut<TransformComponent>( );
    transform.Position = controller.DefaultPosition;
    transform.Rotation = { 0.0f, 0.0f, 0.0f, 1.0f };
    transform.Scale    = { 1.0f, 1.0f, 1.0f };

    auto &cameraComp  = camera.get_mut<CameraComponent>( );
    cameraComp.Active = true;

    const float aspectRatio = static_cast<float>( windowHandle->GetSurface( ).Width ) / static_cast<float>( windowHandle->GetSurface( ).Height );

    cameraComp.View       = CreateViewMatrix( transform.Position, controller.Yaw, controller.Pitch );
    cameraComp.Projection = CreateProjectionMatrix( XMConvertToRadians( 45.0f ), aspectRatio, 0.1f, 100.0f );

    const XMMATRIX viewMatrix = MathConverter::Float4X4ToXMMATRIX( cameraComp.View );
    const XMMATRIX projMatrix = MathConverter::Float4X4ToXMMATRIX( cameraComp.Projection );
    cameraComp.ViewProjection = MathConverter::Float4X4FromXMMATRIX( XMMatrixMultiply( viewMatrix, projMatrix ) );
    cameraComp.Position       = { transform.Position.X, transform.Position.Y, transform.Position.Z, 1.0f };

    world.system<CameraControllerComponent, TransformComponent, CameraComponent>( )
        .kind( flecs::OnUpdate )
        .each(
            []( const flecs::iter &it, size_t, CameraControllerComponent &controller, TransformComponent &transform, CameraComponent &camera )
            {
                const float deltaTime = it.delta_time( );
                const auto &input     = it.world( ).get<InputStateComponent>( );

                if ( controller.EnableRotation )
                {
                    HandleMouseRotation( controller, input );
                }

                HandleMousePanning( controller, input );

                if ( controller.EnableMovement )
                {
                    HandleKeyboardMovement( controller, input, deltaTime );
                    HandleMouseWheelZoom( controller, input );
                }

                UpdateCameraSmoothing( controller, transform );

                if ( ShouldResetCamera( input ) )
                {
                    controller.TargetPosition = controller.DefaultPosition;
                    controller.TargetYaw      = controller.DefaultYaw;
                    controller.TargetPitch    = controller.DefaultPitch;
                }

                UpdateCameraMatrices( camera, transform, controller.Yaw, controller.Pitch );
            } );
}

void SceneViewCameraSystem::HandleMouseRotation( CameraControllerComponent &controller, const InputStateComponent &input )
{
    if ( !input.PressedMouseButtons.contains( MouseButton::Right ) )
    {
        controller.FirstMouse = true;
        controller.Yaw        = controller.TargetYaw;
        controller.Pitch      = controller.TargetPitch;
        return;
    }

    if ( controller.FirstMouse )
    {
        controller.LastMouseX  = static_cast<int>( input.MousePosition.X );
        controller.LastMouseY  = static_cast<int>( input.MousePosition.Y );
        controller.FirstMouse  = false;
        controller.TargetYaw   = controller.Yaw;
        controller.TargetPitch = controller.Pitch;
        return;
    }

    float       xOffset = input.MouseDelta.X * controller.Sensitivity;
    const float yOffset = -input.MouseDelta.Y * controller.Sensitivity;

    if ( std::abs( xOffset ) < 0.001f && std::abs( yOffset ) < 0.001f )
    {
        controller.LastMouseX = static_cast<int>( input.MousePosition.X );
        controller.LastMouseY = static_cast<int>( input.MousePosition.Y );
        return;
    }

    xOffset = -xOffset;
    controller.TargetYaw += xOffset * controller.LookSpeed;
    controller.TargetPitch += yOffset * controller.LookSpeed;
    controller.TargetPitch = std::clamp( controller.TargetPitch, controller.MinPitch, controller.MaxPitch );

    controller.LastMouseX = static_cast<int>( input.MousePosition.X );
    controller.LastMouseY = static_cast<int>( input.MousePosition.Y );
}

void SceneViewCameraSystem::HandleMousePanning( CameraControllerComponent &controller, const InputStateComponent &input )
{
    if ( !input.PressedMouseButtons.contains( MouseButton::Middle ) )
    {
        return;
    }

    const XMVECTOR forward = CalculateCameraForward( controller.Yaw, controller.Pitch );
    const XMVECTOR worldUp = XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f );
    const XMVECTOR right   = XMVector3Normalize( XMVector3Cross( worldUp, forward ) );
    const XMVECTOR up      = XMVector3Cross( forward, right );

    const float panX = input.MouseDelta.X * controller.Sensitivity * 0.05f;
    const float panY = input.MouseDelta.Y * controller.Sensitivity * 0.05f;

    XMVECTOR targetPos        = MathConverter::Float3ToXMVECTOR( controller.TargetPosition );
    targetPos                 = XMVectorAdd( targetPos, XMVectorScale( right, -panX ) );
    targetPos                 = XMVectorAdd( targetPos, XMVectorScale( up, panY ) );
    controller.TargetPosition = MathConverter::Float3FromXMVECTOR( targetPos );
}

void SceneViewCameraSystem::HandleKeyboardMovement( CameraControllerComponent &controller, const InputStateComponent &input, float deltaTime )
{
    XMVECTOR movement = XMVectorZero( );

    const XMVECTOR forward = CalculateCameraForward( controller.Yaw, controller.Pitch );
    const XMVECTOR worldUp = XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f );
    const XMVECTOR right   = XMVector3Normalize( XMVector3Cross( worldUp, forward ) );

    if ( input.PressedKeys.contains( KeyCode::W ) )
    {
        movement = XMVectorAdd( movement, forward );
    }
    if ( input.PressedKeys.contains( KeyCode::S ) )
    {
        movement = XMVectorSubtract( movement, forward );
    }
    if ( input.PressedKeys.contains( KeyCode::A ) )
    {
        movement = XMVectorSubtract( movement, right );
    }
    if ( input.PressedKeys.contains( KeyCode::D ) )
    {
        movement = XMVectorAdd( movement, right );
    }
    if ( input.PressedKeys.contains( KeyCode::Q ) || input.PressedKeys.contains( KeyCode::LCtrl ) )
    {
        movement = XMVectorSubtract( movement, worldUp );
    }
    if ( input.PressedKeys.contains( KeyCode::E ) || input.PressedKeys.contains( KeyCode::Space ) )
    {
        movement = XMVectorAdd( movement, worldUp );
    }

    if ( XMVector3Equal( movement, XMVectorZero( ) ) )
    {
        return;
    }

    movement = XMVector3Normalize( movement );

    float velocity = controller.MoveSpeed;
    if ( input.PressedKeys.contains( KeyCode::LShift ) )
    {
        velocity *= controller.SprintMultiplier;
    }
    velocity *= deltaTime;

    XMVECTOR targetPos        = MathConverter::Float3ToXMVECTOR( controller.TargetPosition );
    targetPos                 = XMVectorAdd( targetPos, XMVectorScale( movement, velocity ) );
    controller.TargetPosition = MathConverter::Float3FromXMVECTOR( targetPos );
}

void SceneViewCameraSystem::HandleMouseWheelZoom( CameraControllerComponent &controller, const InputStateComponent &input )
{
    if ( input.MouseWheel.Y == 0 )
    {
        return;
    }

    const XMVECTOR forward    = CalculateCameraForward( controller.Yaw, controller.Pitch );
    const float    zoomAmount = input.MouseWheel.Y * controller.ScrollSpeed;

    XMVECTOR targetPos        = MathConverter::Float3ToXMVECTOR( controller.TargetPosition );
    targetPos                 = XMVectorAdd( targetPos, XMVectorScale( forward, zoomAmount * controller.MoveSpeed ) );
    controller.TargetPosition = MathConverter::Float3FromXMVECTOR( targetPos );
}

void SceneViewCameraSystem::UpdateCameraSmoothing( CameraControllerComponent &controller, TransformComponent &transform )
{
    XMVECTOR       currentPos = MathConverter::Float3ToXMVECTOR( transform.Position );
    const XMVECTOR targetPos  = MathConverter::Float3ToXMVECTOR( controller.TargetPosition );
    currentPos                = XMVectorLerp( currentPos, targetPos, controller.SmoothFactor );
    transform.Position        = MathConverter::Float3FromXMVECTOR( currentPos );

    controller.Yaw   = controller.Yaw + ( controller.TargetYaw - controller.Yaw ) * controller.SmoothFactor;
    controller.Pitch = controller.Pitch + ( controller.TargetPitch - controller.Pitch ) * controller.SmoothFactor;
}

void SceneViewCameraSystem::UpdateCameraMatrices( CameraComponent &camera, const TransformComponent &transform, const float yaw, const float pitch )
{
    camera.View     = CreateViewMatrix( transform.Position, yaw, pitch );
    camera.Position = Float4{ transform.Position.X, transform.Position.Y, transform.Position.Z, 1.0f };

    const XMMATRIX viewMatrix     = MathConverter::Float4X4ToXMMATRIX( camera.View );
    const XMMATRIX projMatrix     = MathConverter::Float4X4ToXMMATRIX( camera.Projection );
    const XMMATRIX viewProjMatrix = XMMatrixMultiply( viewMatrix, projMatrix );

    camera.ViewProjection = MathConverter::Float4X4FromXMMATRIX( viewProjMatrix );
}

bool SceneViewCameraSystem::ShouldResetCamera( const InputStateComponent &input )
{
    return input.PressedKeys.contains( KeyCode::F ) && ( input.PressedKeys.contains( KeyCode::LCtrl ) || input.PressedKeys.contains( KeyCode::RCtrl ) );
}

XMVECTOR SceneViewCameraSystem::CalculateCameraForward( const float yaw, const float pitch )
{
    return XMVector3Normalize( XMVectorSet( std::cos( yaw ) * std::cos( pitch ), std::sin( pitch ), std::sin( yaw ) * std::cos( pitch ), 0.0f ) );
}

Float4x4 SceneViewCameraSystem::CreateViewMatrix( const Float3 &position, const float yaw, const float pitch )
{
    const XMVECTOR pos     = XMVectorSet( position.X, position.Y, position.Z, 1.0f );
    const XMVECTOR front   = CalculateCameraForward( yaw, pitch );
    const XMVECTOR worldUp = XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f );
    const XMVECTOR target  = XMVectorAdd( pos, front );
    const XMMATRIX view    = XMMatrixLookAtLH( pos, target, worldUp );

    return MathConverter::Float4X4FromXMMATRIX( view );
}

Float4x4 SceneViewCameraSystem::CreateProjectionMatrix( const float fov, const float aspectRatio, const float nearPlane, const float farPlane )
{
    const XMMATRIX proj = XMMatrixPerspectiveFovLH( fov, aspectRatio, nearPlane, farPlane );
    return MathConverter::Float4X4FromXMMATRIX( proj );
}
