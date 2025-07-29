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
#include "DZEngine/GraphicsContext.h"

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

    GraphicsContextDesc graphicsContextDesc{ };
    graphicsContextDesc.WindowHandle = graphicsWindowHandle;
    m_graphicsContext                = std::make_unique<GraphicsContext>( graphicsContextDesc );
}

void App::HandleEvent( const Event &event )
{
    m_graphicsContext->HandleEvent( event );
    if ( event.Type == EventType::Quit )
    {
        m_isRunning = false;
    }
    if ( event.Type == EventType::KeyDown && event.Key.KeyCode == KeyCode::Escape )
    {
        m_isRunning = false;
    }
}

void App::Run( )
{
    InitSystems( );

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

void App::Update( )
{
    uint32_t nextImage = m_graphicsContext->AcquireNextImage( );



}

void App::InitSystems( )
{
}

App::~App( )
{
    Engine::Shutdown( );
}

void App::RunEditor( )
{
}

void App::RunGame( )
{
}
