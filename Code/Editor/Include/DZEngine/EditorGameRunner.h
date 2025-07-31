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

#include "DZEngine/IGameRunner.h"
#include "DZEngine/Rendering/RenderLoop.h"
#include "Editor.h"

namespace DZEngine
{
    class EditorGameRunner final : public IGameRunner
    {
        IGame                                   *m_game;
        GraphicsContext                         *m_graphicsContext;
        std::unique_ptr<RenderLoop>              m_renderLoop;
        std::unique_ptr<Editor>                  m_editor;
        std::unique_ptr<AppContext>              m_appContext;
        std::vector<std::unique_ptr<ISemaphore>> m_renderCompleteSemaphores;

    public:
        explicit EditorGameRunner( const GameRunnerDesc &desc );
        ~EditorGameRunner( ) override;
        void HandleEvent( const Event &event ) override;
        void Update( ) override;
    };
} // namespace DZEngine
