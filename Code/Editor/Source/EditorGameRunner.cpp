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

#include "DZEngine/EditorGameRunner.h"
#include "../Include/DZEngine/Systems/SceneViewCameraSystem.h"

using namespace DZEngine;

EditorGameRunner::EditorGameRunner( const GameRunnerDesc &desc ) : AGameRunner( desc )
{
    SceneViewCameraSystem::Register( m_world->GetWorld( ), desc.Window->GetGraphicsWindowHandle( ) );

    EditorDesc editorDesc{ };
    editorDesc.AppContext = m_appContext.get( );
    editorDesc.Window     = desc.Window;
    m_editor              = std::make_unique<Editor>( editorDesc );

    m_renderCompleteSemaphores.resize( m_graphicsContext->NumFramesInFlight );
    for ( uint32_t i = 0; i < m_graphicsContext->NumFramesInFlight; i++ )
    {
        m_renderCompleteSemaphores[ i ] = std::unique_ptr<ISemaphore>( m_graphicsContext->LogicalDevice->CreateSemaphore( ) );
    }
}

EditorGameRunner::~EditorGameRunner( )
{
}

void EditorGameRunner::HandleEvent( const Event &event )
{
    m_renderLoop->HandleEvent( event );
    m_editor->HandleEvent( event );
    m_world->HandleEvent( event );
    m_game->HandleEvent( event );
}

void EditorGameRunner::Update( )
{
    const FrameState frameState = m_renderLoop->NextFrame( );
    if ( frameState.IsDeviceBusy )
    {
        return;
    }
    m_world->Progress( );
    const GameRenderView gameRenderView = m_editor->GetGameRenderView( frameState.FrameIndex );

    RenderDesc renderDesc{ };
    renderDesc.FrameIndex      = frameState.FrameIndex;
    renderDesc.NotifyFence     = nullptr; // During engine run mode the editor notifies frame completion
    renderDesc.SignalSemaphore = m_renderCompleteSemaphores[ frameState.FrameIndex ].get( );
    renderDesc.RenderTarget    = gameRenderView.RenderTarget;
    renderDesc.Viewport        = gameRenderView.Viewport;

    ISemaphore *gameRenderedSemaphore = nullptr;
    if ( m_game->Render( renderDesc ) )
    {
        gameRenderedSemaphore = m_renderCompleteSemaphores[ frameState.FrameIndex ].get( );
    }

    EditorUpdateDesc updateDesc{ };
    updateDesc.FrameIndex    = frameState.FrameIndex;
    updateDesc.SignalFence   = frameState.NotifyFence;
    updateDesc.GameSemaphore = gameRenderedSemaphore;
    updateDesc.RenderTarget  = frameState.RenderTarget;

    m_editor->Update( updateDesc );

    m_renderLoop->Present( );
}
