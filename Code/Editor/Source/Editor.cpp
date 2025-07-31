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

#include "DZEngine/Editor.h"

using namespace DZEngine;

Editor::Editor( const EditorDesc editorDesc ) : m_appContext( editorDesc.AppContext )
{
}

Editor::~Editor( )
{
}

GameRenderView Editor::GetGameRenderView( uint32_t frameIndex )
{
    GameRenderView renderView{ };

    return renderView;
}

void Editor::HandleEvent( const Event &event )
{
}

void Editor::Update( const EditorUpdateDesc &updateDesc )
{
}
