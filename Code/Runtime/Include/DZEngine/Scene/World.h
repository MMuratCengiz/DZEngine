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
#include <memory>
#include <string>
#include <unordered_map>

#include "DZEngine/Rendering/GraphicsContext.h"
#include "DZEngine/Scene/Scene.h"
#include "DenOfIzGraphics/Input/Event.h"
#include "DenOfIzGraphics/Utilities/StepTimer.h"

namespace DZEngine
{
    struct WorldDesc
    {
        GraphicsContext *GraphicsContext;
    };

    class World
    {
        GraphicsContext *m_graphicsContext;

        flecs::world                                            m_world;
        std::unordered_map<std::string, std::unique_ptr<Scene>> m_scenes;
        Scene                                                  *m_activeScene = nullptr;
        StepTimer                                               m_timer;

    public:
        explicit World( const WorldDesc &desc );
        ~World( );

        Scene *CreateScene( const std::string &name );
        void   RemoveScene( const std::string &name );
        Scene *GetScene( const std::string &name );
        Scene *GetActiveScene( ) const;

        void SetActiveScene( const std::string &name );
        void SetActiveScene( Scene *scene );

        flecs::world       &GetWorld( );
        const flecs::world &GetWorld( ) const;

        void HandleEvent( const Event &event );
        void Progress( );

        template <typename T>
        auto RegisterSystem( ) const
        {
            return m_world.system<T>( );
        }

    private:
        void SetupSceneObservers( );
        void ActivateScene( Scene *scene );
    };
} // namespace DZEngine
