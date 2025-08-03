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
#include "AppContext.h"
#include "DZEngine/AGameRunner.h"
#include "Rendering/GraphicsContext.h"
#include "Rendering/RenderLoop.h"
#include "Scene/World.h"

namespace DZEngine
{
    class GameRunner final : public AGameRunner
    {
    public:
        explicit GameRunner( const GameRunnerDesc &desc );
        ~GameRunner( ) override;
        void HandleEvent( const Event &event ) override;
        void Update( ) override;
    };
} // namespace DZEngine
