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

#include "Assets/AssetBatcher.h"
#include "Assets/AssetBundle.h"
#include "Assets/AssetRegistry.h"
#include "IGame.h"
#include "Rendering/RenderLoop.h"

namespace DZEngine
{
    struct GameRunnerDesc
    {
        Window *Window;
        IGame  *Game;
    };

    // Abstract game runner since context initialization is mostly common
    class AGameRunner
    {
    protected:
        GraphicsWindowHandle          *m_windowHandle;
        IGame                         *m_game;
        GraphicsContext               *m_graphicsContext;
        std::unique_ptr<RenderLoop>    m_renderLoop;
        std::unique_ptr<AssetBundle>   m_assetBundle;
        std::unique_ptr<AssetRegistry> m_assetRegistry;
        std::unique_ptr<AssetBatcher>  m_assetBatcher;
        std::unique_ptr<AppContext>    m_appContext;
        std::unique_ptr<World>         m_world;

    public:
        explicit AGameRunner( const GameRunnerDesc &desc );

        virtual ~AGameRunner( )                        = default;
        virtual void HandleEvent( const Event &event ) = 0;
        virtual void Update( )                         = 0;
    };
} // namespace DZEngine
