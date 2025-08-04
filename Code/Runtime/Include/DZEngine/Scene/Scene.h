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

#include <flecs.h>
#include <string>
#include <filesystem>
#include "DZEngine/Rendering/GraphicsContext.h"

namespace DZEngine
{
    struct SceneDesc
    {
        GraphicsContext *GraphicsContext;
        flecs::world    *World;
        std::string      Name;
    };

    struct ActiveScene
    {
    };

    class Scene
    {
        friend class World;

        std::string   m_name;
        flecs::entity m_root;
        flecs::entity m_pipeline;
        flecs::world *m_world;

    public:
        explicit Scene( const SceneDesc &desc );
        ~Scene( );

        flecs::entity CreateEntity( const std::string &name = "" ) const;
        void          DestroyEntity( flecs::entity entity ) const;

        void Clear( ) const;

        const std::string &GetName( ) const;
        flecs::entity      GetRoot( ) const;
        
        bool LoadFromFile( const std::filesystem::path &filePath );
    };
} // namespace DZEngine
