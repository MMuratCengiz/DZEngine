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

#include "DZEngine/GameRunner.h"

using namespace DZEngine;

GameRunner::GameRunner( const GameRunnerDesc &desc ) : m_windowHandle( desc.Window->GetGraphicsWindowHandle( ) ), m_game( desc.Game )
{
    RenderLoopDesc renderLoopDesc{ };
    renderLoopDesc.WindowHandle = m_windowHandle;
    m_renderLoop                = std::make_unique<RenderLoop>( renderLoopDesc );
    m_graphicsContext           = m_renderLoop->GetGraphicsContext( );

    WorldDesc worldDesc{ };
    worldDesc.GraphicsContext = m_graphicsContext;
    m_world                   = std::make_unique<World>( worldDesc );

    m_appContext                  = std::make_unique<AppContext>( );
    m_appContext->GraphicsContext = m_graphicsContext;
    m_appContext->World           = m_world.get( );
    m_game->Init( m_appContext.get( ) );
}

GameRunner::~GameRunner( )
{
}

void GameRunner::HandleEvent( const Event &event )
{
    m_renderLoop->HandleEvent( event );
    m_game->HandleEvent( event );
}

void GameRunner::Update( )
{
    m_world->Progress( );
    m_game->Update( );

    const FrameState frameState = m_renderLoop->NextFrame( );
    if ( frameState.IsDeviceBusy )
    {
        return;
    }

    RenderDesc renderDesc{ };
    renderDesc.FrameIndex      = frameState.FrameIndex;
    renderDesc.NotifyFence     = frameState.NotifyFence;
    renderDesc.SignalSemaphore = nullptr;
    renderDesc.RenderTarget    = frameState.RenderTarget;

    const GraphicsWindowSurface surface = m_windowHandle->GetSurface( );
    renderDesc.Viewport                 = { 0.0f, 0.0f, static_cast<float>( surface.Width ), static_cast<float>( surface.Height ) };
    if ( m_game->Render( renderDesc ) )
    {
        m_renderLoop->Present( );
    }
}
