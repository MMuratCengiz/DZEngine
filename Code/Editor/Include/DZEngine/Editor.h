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
#include "ImGuiBackend.h"
#include "DZEngine/Style/EditorStyle.h"
#include "DZEngine/Components/AssetBrowser.h"
#include <memory>

using namespace DenOfIz;

namespace DZEngine
{
    struct EditorDesc
    {
        AppContext *AppContext;
    };

    struct GameRenderView
    {
        ITextureResource *RenderTarget;
        Viewport          Viewport;
    };

    struct EditorUpdateDesc
    {
        uint32_t          FrameIndex;
        IFence           *SignalFence;   // Can be null
        ISemaphore       *GameSemaphore; // Can be null
        ITextureResource *RenderTarget;
    };

    class Editor
    {
        AppContext                    *m_appContext;
        GraphicsContext               *m_graphicsContext;
        std::unique_ptr<ImGuiRenderer> m_imguiRenderer;
        ImFont                        *m_font = nullptr;
        StepTimer                      m_stepTimer;

        std::unique_ptr<ICommandListPool> m_commandListPool;
        std::vector<ICommandList *>       m_commandLists;

        std::unique_ptr<AssetBrowser> m_assetBrowser;
        bool m_showDemoWindow = false;
        bool m_firstFrame = true;
        ImGuiID m_dockspaceId = 0;

    public:
        explicit Editor( EditorDesc editorDesc );
        ~Editor( );
        GameRenderView GetGameRenderView( uint32_t frameIndex ) const;
        void           HandleEvent( const Event &event ) const;
        void           Update( EditorUpdateDesc updateDesc );

    private:
        void RenderMenuBar();
        void RenderMainUI();
        void SetupDefaultDockingLayout( ) const;
    };
} // namespace DZEngine
