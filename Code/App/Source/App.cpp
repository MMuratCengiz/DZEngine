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

#include "DZEngine/App.h"

#include "DZEngine/DummyGame.h"
#include "DZEngine/GameRunner.h"

using namespace DZEngine;

App::App( AppDesc appDesc )
{
    const EngineDesc engineDesc{ };
    Engine::Init( engineDesc );

    WindowDesc windowDesc{ };
    windowDesc.Width    = Display::GetPrimaryDisplay( ).Size.Width * 0.8f;
    windowDesc.Height   = Display::GetPrimaryDisplay( ).Size.Height * 0.8f;
    windowDesc.Position = WindowPosition::Centered;
    m_window            = std::make_unique<Window>( windowDesc );

    const auto graphicsWindowHandle = m_window->GetGraphicsWindowHandle( );

    m_game = std::make_unique<DummyGame>( ); // TODO

    GameRunnerDesc gameRunnerDesc{ };
    gameRunnerDesc.WindowHandle = graphicsWindowHandle;
    gameRunnerDesc.Game         = m_game.get( );

#ifdef DZ_RUN_MODE_EDITOR
    m_gameRunner = std::make_unique<EditorGameRunner>( gameRunnerDesc );
#else
    m_gameRunner = std::make_unique<GameRunner>( gameRunnerDesc );
#endif
}

void App::HandleEvent( const Event &event )
{
    if ( event.Type == EventType::Quit )
    {
        m_isRunning = false;
    }
    if ( event.Type == EventType::KeyDown && event.Key.KeyCode == KeyCode::Escape )
    {
        m_isRunning = false;
    }
    m_gameRunner->HandleEvent( event );
}

void App::Run( )
{
    Event       event;
    InputSystem inputSystem{ };
    while ( m_isRunning )
    {
        while ( InputSystem::PollEvent( event ) )
        {
            if ( event.Type == EventType::Quit )
            {
                m_isRunning = false;
            }
            HandleEvent( event );
        }
        Update( );
    }
}

void App::Update( ) const
{
    m_gameRunner->Update( );
}

App::~App( )
{
    Engine::Shutdown( );
}
