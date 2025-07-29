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

#include <memory>
#include "DenOfIzGraphics/Input/Controller.h"
#include "DenOfIzGraphics/Input/Event.h"
#include "DenOfIzGraphics/Input/Window.h"
#include "DenOfIzGraphics/Utilities/Common.h"
#include "DenOfIzGraphics/Utilities/Engine.h"
#include "DenOfIzGraphics/Utilities/Interop.h"

namespace DenOfIz
{
    struct DZ_API MouseCoords
    {
        float X;    // SDL3: Floating-point coordinates for sub-pixel precision
        float Y;
    };

    class InputSystem
    {
        struct Impl;
        std::unique_ptr<Impl> m_impl;

    public:
        DZ_API InputSystem( );
        DZ_API ~InputSystem( );

        DZ_API InputSystem( const InputSystem & )            = delete;
        DZ_API InputSystem &operator=( const InputSystem & ) = delete;

        DZ_API InputSystem( InputSystem &&other ) noexcept;
        DZ_API InputSystem &operator=( InputSystem &&other ) noexcept;

        DZ_API static bool PollEvent( Event &outEvent );
        DZ_API static bool WaitEvent( Event &outEvent );
        DZ_API static bool WaitEventTimeout( Event &outEvent, int timeout );
        DZ_API static void PumpEvents( );
        DZ_API static void FlushEvents( EventType minType, EventType maxType );
        DZ_API static void PushEvent( const Event &event );

        // Keyboard
        DZ_API static KeyState      GetKeyState( KeyCode key );
        DZ_API static bool          IsKeyPressed( KeyCode key );
        DZ_API static uint32_t      GetModState( );
        DZ_API static void          SetModState( const uint32_t &modifiers );
        DZ_API static KeyCode       GetKeyFromName( const InteropString &name );
        DZ_API static InteropString GetKeyName( KeyCode key );
        DZ_API static InteropString GetScancodeName( uint32_t scancode );

        // Mouse
        DZ_API static MouseCoords GetMouseState( );
        DZ_API static MouseCoords GetGlobalMouseState( );
        DZ_API static uint32_t    GetMouseButtons( );
        DZ_API static MouseCoords GetRelativeMouseState( );
        DZ_API static void        WarpMouseInWindow( const Window &window, float x, float y );  // SDL3: Float coordinates
        DZ_API static bool        GetRelativeMouseMode( uint32_t windowId );
        DZ_API static void        SetRelativeMouseMode( uint32_t windowId, bool enabled );
        DZ_API static void        CaptureMouse( bool enabled );
        DZ_API static bool        GetMouseFocus( uint32_t windowID );

        // Cursor
        DZ_API static void ShowCursor( bool show );
        DZ_API static bool IsCursorShown( );

        DZ_API static void StartTextInput( );
        DZ_API static void StopTextInput( );
        DZ_API static bool IsTextInputActive( );
        DZ_API static void SetTextInputRect( const Window &window, int x, int y, int w, int h );

        // Controller
        DZ_API [[nodiscard]] static Int32Array GetConnectedControllerIndices( );
        DZ_API [[nodiscard]] static int        GetNumControllers( );
        DZ_API bool                                   OpenController( int playerIndex, int controllerIndex ) const;
        DZ_API void                                   CloseController( int playerIndex ) const;
        DZ_API [[nodiscard]] bool                     IsControllerConnected( int playerIndex ) const;
        DZ_API [[nodiscard]] Controller              *GetController( int playerIndex );
        DZ_API [[nodiscard]] const Controller        *GetController( int playerIndex ) const;
        DZ_API [[nodiscard]] bool                     IsControllerButtonPressed( int playerIndex, ControllerButton button ) const;
        DZ_API [[nodiscard]] int16_t                  GetControllerAxisValue( int playerIndex, ControllerAxis axis ) const;
        DZ_API [[nodiscard]] InteropString            GetControllerName( int playerIndex ) const;
        DZ_API [[nodiscard]] bool                     SetControllerRumble( int playerIndex, uint16_t lowFrequency, uint16_t highFrequency, uint32_t durationMs ) const;
    };

} // namespace DenOfIz
