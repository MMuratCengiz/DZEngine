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

#include "DZEngine/AppContext.h"
#include "DZEngine/Editor.h"

namespace DZEngine
{
    struct AppDesc
    {
    };

    class App
    {
        std::unique_ptr<Window>               m_window;
        std::unique_ptr<GraphicsWindowHandle> m_windowHandle;
        std::unique_ptr<GraphicsContext>      m_graphicsContext;
        std::unique_ptr<AppContext>           m_appContext;

        bool m_isRunning = true;

        std::unique_ptr<Editor> m_editor;

    public:
        explicit App( AppDesc launchDesc );
        void HandleEvent( const Event &event );
        void Update( );
        void Run( );
        void InitSystems( );
        ~App( );

    private:
        void RunEditor( );
        void RunGame( );
    };
} // namespace DZEngine
