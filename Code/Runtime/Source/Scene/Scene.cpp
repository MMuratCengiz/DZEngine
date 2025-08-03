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

#include "DZEngine/Scene/Scene.h"

using namespace DZEngine;

Scene::Scene( const SceneDesc &desc ) : m_world( desc.World )
{
    m_name     = desc.Name;
    m_root     = desc.World->entity( desc.Name.c_str( ) );
    m_pipeline = desc.World->pipeline( ).with<ActiveScene>( ).build( );
}

Scene::~Scene( )
{
    Clear( );
}

flecs::entity Scene::CreateEntity( const std::string &name ) const
{
    const auto entity = m_world->entity( name.c_str( ) );
    entity.child_of( m_root );
    return entity;
}

void Scene::DestroyEntity( const flecs::entity entity ) const
{
    if ( entity.has( flecs::ChildOf, m_root ) )
    {
        entity.destruct( );
    }
}

void Scene::Clear( ) const
{
    m_world->defer(
        [ this ]
        {
            m_world->each(
                [ this ]( const flecs::entity e )
                {
                    if ( e.has( flecs::ChildOf, m_root ) )
                    {
                        e.destruct( );
                    }
                } );
        } );
}

const std::string &Scene::GetName( ) const
{
    return m_name;
}

flecs::entity Scene::GetRoot( ) const
{
    return m_root;
}
