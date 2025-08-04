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

#include "DZEngine/AGameRunner.h"

using namespace DZEngine;
using namespace DenOfIz;

AGameRunner::AGameRunner( const GameRunnerDesc &desc ) : m_windowHandle( desc.Window->GetGraphicsWindowHandle( ) ), m_game( desc.Game )
{
    RenderLoopDesc renderLoopDesc{ };
    renderLoopDesc.WindowHandle = desc.Window->GetGraphicsWindowHandle( );
    m_renderLoop                = std::make_unique<RenderLoop>( renderLoopDesc );
    m_graphicsContext           = m_renderLoop->GetGraphicsContext( );

    m_appContext                  = std::make_unique<AppContext>( );
    m_appContext->NumFrames       = 3;
    m_appContext->GraphicsContext = m_graphicsContext;

    WorldDesc worldDesc{ };
    worldDesc.GraphicsContext = m_graphicsContext;
    m_world                   = std::make_unique<World>( worldDesc );
    m_appContext->World       = m_world.get( );

    m_assetBundle   = std::make_unique<AssetBundle>( "Assets" );
    m_assetRegistry = std::make_unique<AssetRegistry>( "AssetRegistry.json" );

    AssetBatcherDesc batcherDesc{ };
    batcherDesc.GraphicsContext = m_graphicsContext;
    batcherDesc.AssetBundle     = m_assetBundle.get( );
    batcherDesc.AssetRegistry   = m_assetRegistry.get( );
    m_assetBatcher              = std::make_unique<AssetBatcher>( batcherDesc );
    m_appContext->AssetBatcher  = m_assetBatcher.get( );

    m_game->Init( m_appContext.get( ) );

    m_world->GetWorld( ).set_ctx( m_appContext.get( ) );
}
