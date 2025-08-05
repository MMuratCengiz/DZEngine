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

#include <flecs.h>

#include "DZEngine/Components/CameraComponent.h"
#include "DZEngine/Components/CameraControllerComponent.h"
#include "DZEngine/Components/InputStateComponent.h"
#include "DZEngine/Components/TransformComponent.h"
#include "DZEngine/Math/MathConverter.h"
#include "DenOfIzGraphics/Backends/Common/GraphicsWindowHandle.h"

using namespace DenOfIz;

namespace DZEngine
{
    class SceneViewCameraSystem
    {
    public:
        static void Register( const flecs::world &world, const GraphicsWindowHandle *windowHandle );

        static Float4x4 CreateViewMatrix( const Float3 &position, float yaw, float pitch );
        static Float4x4 CreateProjectionMatrix( float fov, float aspectRatio, float nearPlane, float farPlane );

    private:
        static void              HandleMouseRotation( CameraControllerComponent &controller, const InputStateComponent &input );
        static void              HandleMousePanning( CameraControllerComponent &controller, const InputStateComponent &input );
        static void              HandleKeyboardMovement( CameraControllerComponent &controller, const InputStateComponent &input, float deltaTime );
        static void              HandleMouseWheelZoom( CameraControllerComponent &controller, const InputStateComponent &input );
        static void              UpdateCameraSmoothing( CameraControllerComponent &controller, TransformComponent &transform );
        static void              UpdateCameraMatrices( CameraComponent &camera, const TransformComponent &transform, float yaw, float pitch );
        static DirectX::XMVECTOR CalculateCameraForward( float yaw, float pitch );
        static bool              ShouldResetCamera( const InputStateComponent &input );
    };
} // namespace DZEngine
