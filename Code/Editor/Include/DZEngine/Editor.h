
#pragma once

#include <memory>
#include "DZEngine/AppContext.h"
#include "DZEngine/Components/AssetBrowser.h"
#include "DZEngine/Components/SceneViewRenderer.h"
#include "DZEngine/Style/EditorStyle.h"
#include "ImGuiBackend.h"

using namespace DenOfIz;

namespace DZEngine
{
    struct EditorDesc
    {
        AppContext *AppContext;
        Window     *Window;
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
        
        std::unique_ptr<ICommandListPool> m_viewportCommandListPool;
        std::vector<ICommandList *>       m_viewportCommandLists;
        std::unique_ptr<ISemaphore>       m_viewportSemaphore;

        std::unique_ptr<AssetBrowser>      m_assetBrowser;
        std::unique_ptr<SceneViewRenderer> m_sceneViewRenderer;
        bool                               m_showDemoWindow     = false;
        bool                               m_firstFrame         = true;
        ImGuiID                            m_dockspaceId        = 0;
        ImTextureID                        m_sceneViewTextureId = 0;
        ImTextureID                        m_gameViewTextureId  = 0;

        std::vector<std::unique_ptr<ITextureResource>> m_gameRenderTargets;
        Viewport                                       m_gameViewport;

    public:
        explicit Editor( EditorDesc editorDesc );
        ~Editor( );
        GameRenderView GetGameRenderView( uint32_t frameIndex ) const;
        void           HandleEvent( const Event &event ) const;
        void           Update( const EditorUpdateDesc &updateDesc );

    private:
        void RenderMenuBar( );
        void RenderMainUI( uint32_t frameIndex );
        void SetupDefaultDockingLayout( ) const;
        void CreateGameRenderTargets( );
    };
} // namespace DZEngine
