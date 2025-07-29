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

#include "DenOfIzGraphics/Input/InputData.h"
#include "DenOfIzGraphics/Utilities/Interop.h"

namespace DenOfIz
{
    struct DZ_API CommonEventData
    {
        uint32_t Timestamp;
        uint32_t WindowID;
    };

    struct DZ_API KeyboardEventData
    {
        CommonEventData Common;
        KeyState        State;
        uint32_t        Repeat;
        KeyCode         KeyCode;
        uint32_t        Mod;
        uint32_t        ScanCode;
    };

    struct DZ_API TextEditingEventData
    {
        CommonEventData Common;
        char            Text[ 32 ];
        int32_t         Start;
        int32_t         Length;
    };

    struct DZ_API TextInputEventData
    {
        CommonEventData Common;
        char            Text[ 32 ];
    };

    struct DZ_API MouseButtonState
    {
        bool LeftButton   = false;
        bool MiddleButton = false;
        bool RightButton  = false;
        bool X1Button     = false;
        bool X2Button     = false;
    };

    struct DZ_API MouseMotionEventData
    {
        CommonEventData  Common;
        uint32_t         MouseID;
        MouseButtonState Buttons;
        float            X;              // SDL3: Floating-point coordinates for sub-pixel precision
        float            Y;
        float            RelX;           // SDL3: Floating-point relative motion
        float            RelY;
    };

    struct DZ_API MouseButtonEventData
    {
        CommonEventData Common;
        uint32_t        MouseID;
        MouseButton     Button;
        KeyState        State;
        uint32_t        Clicks;
        float           X;               // SDL3: Floating-point coordinates
        float           Y;
    };

    struct DZ_API MouseWheelEventData
    {
        CommonEventData     Common;
        uint32_t            MouseID;
        float               X;           // SDL3: Floating-point wheel delta
        float               Y;
        MouseWheelDirection Direction;
    };

    struct DZ_API WindowEventData
    {
        CommonEventData Common;
        WindowEventType Event;
        int32_t         Data1;
        int32_t         Data2;
    };

    struct DZ_API ControllerAxisEventData
    {
        CommonEventData Common;
        uint32_t        JoystickID;
        ControllerAxis  Axis;
        int16_t         Value;
    };

    struct DZ_API ControllerButtonEventData
    {
        CommonEventData  Common;
        uint32_t         JoystickID;
        ControllerButton Button;
        KeyState         State;
    };

    struct DZ_API ControllerDeviceEventData
    {
        CommonEventData Common;
        uint32_t        JoystickID;
    };

    struct DZ_API QuitEventData
    {
        CommonEventData Common;
    };

    struct DZ_API Event
    {
        EventType Type;

        union
        {
            CommonEventData           Common;
            KeyboardEventData         Key;
            TextEditingEventData      Edit;
            TextInputEventData        Text;
            MouseMotionEventData      MouseMotion;
            MouseButtonEventData      MouseButton;
            MouseWheelEventData       MouseWheel;
            WindowEventData           Window;
            ControllerAxisEventData   ControllerAxis;
            ControllerButtonEventData ControllerButton;
            ControllerDeviceEventData ControllerDevice;
            QuitEventData             Quit;
        };

        Event( ) : Type( EventType::None )
        {
            memset( &Common, 0, sizeof( Common ) );
        }
    };

} // namespace DenOfIz
