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
#include "DZEngine/Components/InputStateComponent.h"
#include "DenOfIzGraphics/Input/Event.h"

namespace DZEngine
{
    class InputSystem
    {
    public:
        static void Register( const flecs::world &world );
        static void HandleEvent( const flecs::world &world, const Event &event );

    private:
        static void UpdateInputState( InputStateComponent &input, const Event &event );
    };
} // namespace DZEngine
