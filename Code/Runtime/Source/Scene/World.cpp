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

#include "DZEngine/Scene/World.h"
#include "DZEngine/Input/InputSystem.h"

#include <spdlog/spdlog.h>

using namespace DZEngine;

World::World( const WorldDesc &desc ) : m_graphicsContext( desc.GraphicsContext )
{
    m_world.component<ActiveScene>( ).add( flecs::Exclusive );
    SetupSceneObservers( );
    
    // Register input system
    InputSystem::Register( m_world );
}

World::~World( )
{
    for ( const auto &scene : m_scenes | std::views::values )
    {
        scene->Clear( );
    }
}

void World::SetupSceneObservers( )
{
    m_world.observer<ActiveScene>( )
        .event( flecs::OnAdd )
        .each(
            [ this ]( const flecs::entity e, ActiveScene )
            {
                for ( const auto &scene : m_scenes | std::views::values )
                {
                    if ( scene->GetRoot( ) == e )
                    {
                        ActivateScene( scene.get( ) );
                        break;
                    }
                }
            } );
}

Scene *World::CreateScene( const std::string &name )
{
    if ( m_scenes.contains( name ) )
    {
        return m_scenes[ name ].get( );
    }

    SceneDesc sceneDesc{ };
    sceneDesc.Name            = name;
    sceneDesc.World           = &m_world;
    sceneDesc.GraphicsContext = m_graphicsContext;
    auto scene                = std::make_unique<Scene>( sceneDesc );

    Scene *scenePtr  = scene.get( );
    m_scenes[ name ] = std::move( scene );

    return scenePtr;
}

void World::RemoveScene( const std::string &name )
{
    if ( const auto it = m_scenes.find( name ); it != m_scenes.end( ) )
    {
        const Scene *scene = it->second.get( );
        if ( m_activeScene == scene )
        {
            m_activeScene = nullptr;
        }

        scene->Clear( );
        m_scenes.erase( it );
    }
}

Scene *World::GetScene( const std::string &name )
{
    const auto it = m_scenes.find( name );
    if ( it == m_scenes.end( ) )
    {
        spdlog::error( "Scene \"{}\" not found", name );
        return nullptr;
    }
    return it->second.get( );
}

Scene *World::GetActiveScene( ) const
{
    return m_activeScene;
}

void World::SetActiveScene( const std::string &name )
{
    if ( Scene *scene = GetScene( name ) )
    {
        SetActiveScene( scene );
    }
}

void World::SetActiveScene( Scene *scene )
{
    if ( !scene || m_activeScene == scene )
    {
        return;
    }

    m_world.defer(
        [ this, scene ]
        {
            if ( m_activeScene )
            {
                m_activeScene->GetRoot( ).remove<ActiveScene>( );
            }

            m_activeScene = scene;
            scene->GetRoot( ).add<ActiveScene>( );

            m_world.set_pipeline( scene->m_pipeline );
        } );
}

flecs::world &World::GetWorld( )
{
    return m_world;
}

const flecs::world &World::GetWorld( ) const
{
    return m_world;
}

void World::HandleEvent( const DenOfIz::Event &event )
{
    InputSystem::HandleEvent( m_world, event );
}

void World::Progress( )
{
    m_timer.Tick( );
    if ( !m_world.progress( m_timer.GetDeltaTime( ) ) )
    {
        // ?
    }
}

void World::ActivateScene( Scene *scene )
{
    if ( m_activeScene && m_activeScene != scene )
    {
        m_activeScene->Clear( );
    }

    m_activeScene = scene;
}
