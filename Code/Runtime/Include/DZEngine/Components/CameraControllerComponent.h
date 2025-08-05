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
        // Movement settings
        float MoveSpeed   = 10.0f;
        float LookSpeed   = 0.005f;
        float ScrollSpeed = 0.1f;
        float SprintMultiplier = 3.0f;
        
        // Smoothing settings
        float SmoothFactor = 0.25f;
        float Sensitivity = 0.5f;

        // Current state
        Float3 Velocity = { 0, 0, 0 };
        float  Yaw      = -1.5708f; // -90 degrees (looking forward along +Z)
        float  Pitch    = 0.0f;

        // Target state (for smooth interpolation)
        Float3 TargetPosition = { 0, 0, 5.0f };
        float  TargetYaw      = -1.5708f; // -90 degrees
        float  TargetPitch    = 0.0f;

        // Rotation limits
        float MinPitch = -1.396f; // ~-80 degrees
        float MaxPitch = 1.396f;  // ~80 degrees

        // Control flags
        bool EnableMovement = true;
        bool EnableRotation = true;
        
        // Mouse state tracking
        bool FirstMouse = true;
        int LastMouseX = 0;
        int LastMouseY = 0;
        
        // Default values for reset
        Float3 DefaultPosition = { 0, 0, 5.0f };
        float DefaultYaw = -1.5708f;
        float DefaultPitch = 0.0f;
    };
} // namespace DZEngine
