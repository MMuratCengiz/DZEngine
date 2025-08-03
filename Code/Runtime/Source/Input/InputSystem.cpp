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

#include "DZEngine/Input/InputSystem.h"

using namespace DZEngine;

void InputSystem::Register( const flecs::world &world )
{
    world.set<InputStateComponent>( { } );

    world.system<InputStateComponent>( )
        .kind( flecs::PreUpdate )
        .each(
            []( InputStateComponent &input )
            {
                input.MouseDelta = { 0, 0 };
                input.MouseWheel = { 0, 0 };
            } );
}

void InputSystem::HandleEvent( const flecs::world &world, const Event &event )
{
    auto input = world.get_mut<InputStateComponent>( );
    UpdateInputState( input, event );
}

void InputSystem::UpdateInputState( InputStateComponent &input, const Event &event )
{
    switch ( event.Type )
    {
    case EventType::KeyDown:
        input.PressedKeys.insert( event.Key.KeyCode );
        break;

    case EventType::KeyUp:
        input.PressedKeys.erase( event.Key.KeyCode );
        break;

    case EventType::MouseMotion:
        {
            const Float2 newPos = { event.MouseMotion.X, event.MouseMotion.Y };
            input.MouseDelta.X += event.MouseMotion.RelX;
            input.MouseDelta.Y += event.MouseMotion.RelY;
            input.MousePosition = newPos;
        }
        break;

    case EventType::MouseButtonDown:
        input.PressedMouseButtons.insert( event.MouseButton.Button );
        break;

    case EventType::MouseButtonUp:
        input.PressedMouseButtons.erase( event.MouseButton.Button );
        break;

    case EventType::MouseWheel:
        input.MouseWheel.X += event.MouseWheel.X;
        input.MouseWheel.Y += event.MouseWheel.Y;
        break;

    default:
        break;
    }
}
