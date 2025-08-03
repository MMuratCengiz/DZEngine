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

#include "DenOfIzGraphics/Utilities/InteropMath.h"

using namespace DenOfIz;

namespace DZEngine
{
    struct CameraControllerComponent
    {
        float MoveSpeed   = 10.0f;
        float LookSpeed   = 0.005f;
        float ScrollSpeed = 0.1f;

        Float3 Velocity = { 0, 0, 0 };
        float  Yaw      = 0.0f;
        float  Pitch    = 0.0f;

        float MinPitch = -1.396f; // ~-80 degrees
        float MaxPitch = 1.396f;  // ~80 degrees

        bool EnableMovement = true;
        bool EnableRotation = true;
    };
} // namespace DZEngine
