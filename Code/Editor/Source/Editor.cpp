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
#include <imgui_internal.h>
#include "DZEngine/Style/ImGuiFonts.h"

using namespace DZEngine;

Editor::Editor( const EditorDesc editorDesc ) : m_appContext( editorDesc.AppContext ), m_graphicsContext( editorDesc.AppContext->GraphicsContext )
{
    ImGuiBackendDesc backendDesc{ };
    backendDesc.LogicalDevice      = m_graphicsContext->LogicalDevice;
    backendDesc.RenderTargetFormat = Format::B8G8R8A8Unorm;
    backendDesc.NumFrames          = m_graphicsContext->NumFramesInFlight;
    backendDesc.Viewport           = m_graphicsContext->SwapChain->GetViewport( );
    m_imguiRenderer                = std::make_unique<ImGuiRenderer>( backendDesc );

    ImGuiIO &io = ImGui::GetIO( );
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    ImGuiFonts::Instance( ).Initialize( { } );
    ImGuiFonts::Instance( ).LoadFonts( );
    m_imguiRenderer->RecreateFonts( );

    EditorStyle::ApplyTheme( Theme::Dark );

    m_assetBrowser = std::make_unique<AssetBrowser>( );

    SceneViewRendererDesc sceneViewDesc{ };
    sceneViewDesc.LogicalDevice     = m_graphicsContext->LogicalDevice;
    sceneViewDesc.Viewport          = m_graphicsContext->SwapChain->GetViewport( );
    sceneViewDesc.NumFramesInFlight = m_graphicsContext->NumFramesInFlight;
    m_sceneViewRenderer             = std::make_unique<SceneViewRenderer>( sceneViewDesc );

    WorldEditorViewDesc worldEditorDesc{ };
    worldEditorDesc.LogicalDevice     = m_graphicsContext->LogicalDevice;
    worldEditorDesc.Viewport          = m_graphicsContext->SwapChain->GetViewport( );
    worldEditorDesc.NumFramesInFlight = m_graphicsContext->NumFramesInFlight;
    m_worldEditorView                 = std::make_unique<WorldEditorView>( worldEditorDesc );

    m_gameViewport = m_graphicsContext->SwapChain->GetViewport( );
    CreateGameRenderTargets( );

    CommandListPoolDesc commandListPoolDesc{ };
    commandListPoolDesc.CommandQueue    = m_graphicsContext->GraphicsQueue;
    commandListPoolDesc.NumCommandLists = m_graphicsContext->NumFramesInFlight;
    m_commandListPool                   = std::unique_ptr<ICommandListPool>( m_graphicsContext->LogicalDevice->CreateCommandListPool( commandListPoolDesc ) );

    const ICommandListArray commandLists = m_commandListPool->GetCommandLists( );
    m_commandLists.resize( commandLists.NumElements );
    for ( uint32_t i = 0; i < commandLists.NumElements; ++i )
    {
        m_commandLists[ i ] = commandLists.Elements[ i ];
    }

    CommandListPoolDesc viewportCmdListPoolDesc{ };
    viewportCmdListPoolDesc.CommandQueue    = m_graphicsContext->GraphicsQueue;
    viewportCmdListPoolDesc.NumCommandLists = m_graphicsContext->NumFramesInFlight;
    m_viewportCommandListPool               = std::unique_ptr<ICommandListPool>( m_graphicsContext->LogicalDevice->CreateCommandListPool( viewportCmdListPoolDesc ) );

    const ICommandListArray viewportCmdLists = m_viewportCommandListPool->GetCommandLists( );
    m_viewportCommandLists.resize( viewportCmdLists.NumElements );
    for ( uint32_t i = 0; i < viewportCmdLists.NumElements; ++i )
    {
        m_viewportCommandLists[ i ] = viewportCmdLists.Elements[ i ];
    }

    m_viewportSemaphore = std::unique_ptr<ISemaphore>( m_graphicsContext->LogicalDevice->CreateSemaphore( ) );
}

Editor::~Editor( )
{
    if ( m_sceneViewTextureId != 0 && m_imguiRenderer )
    {
        m_imguiRenderer->RemoveTexture( m_sceneViewTextureId );
    }
    if ( m_gameViewTextureId != 0 && m_imguiRenderer )
    {
        m_imguiRenderer->RemoveTexture( m_gameViewTextureId );
    }
}

GameRenderView Editor::GetGameRenderView( uint32_t frameIndex ) const
{
    GameRenderView renderView{ };

    if ( frameIndex < m_gameRenderTargets.size( ) && m_gameRenderTargets[ frameIndex ] )
    {
        renderView.RenderTarget = m_gameRenderTargets[ frameIndex ].get( );
        renderView.Viewport     = m_gameViewport;
    }

    return renderView;
}

void Editor::HandleEvent( const Event &event ) const
{
    m_imguiRenderer->ProcessEvent( event );
}

void Editor::Update( EditorUpdateDesc updateDesc )
{
    m_stepTimer.Tick( );

    if ( m_worldEditorView )
    {
        m_worldEditorView->Update( );
    }

    ICommandList *viewportCmdList = m_viewportCommandLists[ updateDesc.FrameIndex ];

    viewportCmdList->Begin( );
    m_sceneViewRenderer->Render( viewportCmdList, updateDesc.FrameIndex );
    viewportCmdList->End( );

    ISemaphore             *viewportSemaphore = m_viewportSemaphore.get( );
    ExecuteCommandListsDesc viewportExecuteDesc{ };
    viewportExecuteDesc.CommandLists.Elements        = &viewportCmdList;
    viewportExecuteDesc.CommandLists.NumElements     = 1;
    viewportExecuteDesc.SignalSemaphores.Elements    = &viewportSemaphore;
    viewportExecuteDesc.SignalSemaphores.NumElements = 1;
    m_graphicsContext->GraphicsQueue->ExecuteCommandLists( viewportExecuteDesc );

    ICommandList *commandList = m_commandLists[ updateDesc.FrameIndex ];

    commandList->Begin( );

    ITextureResource *renderTarget = updateDesc.RenderTarget;
    m_graphicsContext->ResourceTracking->TransitionTexture( commandList, renderTarget, ResourceUsage::RenderTarget );

    const auto viewport = m_graphicsContext->SwapChain->GetViewport( );
    m_imguiRenderer->NewFrame( static_cast<uint32_t>( viewport.Width ), static_cast<uint32_t>( viewport.Height ), m_stepTimer.GetDeltaTime( ) );

    RenderMainUI( updateDesc.FrameIndex );

    ImGui::Render( );

    m_imguiRenderer->Render( renderTarget, commandList, updateDesc.FrameIndex );
    m_graphicsContext->ResourceTracking->TransitionTexture( commandList, renderTarget, ResourceUsage::Present );

    commandList->End( );

    ExecuteCommandListsDesc executeCommandListsDesc{ };
    executeCommandListsDesc.CommandLists.Elements    = &commandList;
    executeCommandListsDesc.CommandLists.NumElements = 1;
    if ( updateDesc.SignalFence )
    {
        executeCommandListsDesc.Signal = updateDesc.SignalFence;
    }

    std::vector<ISemaphore *> waitSemaphores;
    if ( updateDesc.GameSemaphore )
    {
        waitSemaphores.push_back( updateDesc.GameSemaphore );
    }
    waitSemaphores.push_back( m_viewportSemaphore.get( ) );

    executeCommandListsDesc.WaitSemaphores.Elements    = waitSemaphores.data( );
    executeCommandListsDesc.WaitSemaphores.NumElements = static_cast<uint32_t>( waitSemaphores.size( ) );

    m_graphicsContext->GraphicsQueue->ExecuteCommandLists( executeCommandListsDesc );
}

void Editor::RenderMenuBar( )
{
    if ( ImGui::BeginMenuBar( ) )
    {
        if ( ImGui::BeginMenu( "File" ) )
        {
            if ( ImGui::MenuItem( "New Scene", "Ctrl+N" ) )
            {
            }
            if ( ImGui::MenuItem( "Open Scene", "Ctrl+O" ) )
            {
            }
            if ( ImGui::MenuItem( "Save Scene", "Ctrl+S" ) )
            {
            }
            ImGui::Separator( );
            if ( ImGui::MenuItem( "Exit", "Alt+F4" ) )
            {
            }
            ImGui::EndMenu( );
        }

        if ( ImGui::BeginMenu( "Edit" ) )
        {
            if ( ImGui::MenuItem( "Undo", "Ctrl+Z" ) )
            {
            }
            if ( ImGui::MenuItem( "Redo", "Ctrl+Y" ) )
            {
            }
            ImGui::Separator( );
            if ( ImGui::MenuItem( "Cut", "Ctrl+X" ) )
            {
            }
            if ( ImGui::MenuItem( "Copy", "Ctrl+C" ) )
            {
            }
            if ( ImGui::MenuItem( "Paste", "Ctrl+V" ) )
            {
            }
            ImGui::EndMenu( );
        }

        if ( ImGui::BeginMenu( "View" ) )
        {
            ImGui::MenuItem( "Asset Browser", nullptr, nullptr );
            ImGui::MenuItem( "Inspector", nullptr, nullptr );
            ImGui::MenuItem( "Hierarchy", nullptr, nullptr );
            ImGui::MenuItem( "Scene View", nullptr, nullptr );
            ImGui::MenuItem( "Game View", nullptr, nullptr );
            ImGui::Separator( );
            ImGui::MenuItem( "Console", nullptr, nullptr );
            ImGui::Separator( );
            if ( ImGui::MenuItem( "Demo Window", nullptr, &m_showDemoWindow ) )
            {
            }
            ImGui::EndMenu( );
        }

        if ( ImGui::BeginMenu( "Window" ) )
        {
            if ( ImGui::BeginMenu( "Layouts" ) )
            {
                if ( ImGui::MenuItem( "Default" ) )
                {
                }
                if ( ImGui::MenuItem( "2 by 3" ) )
                {
                }
                if ( ImGui::MenuItem( "4 Split" ) )
                {
                }
                if ( ImGui::MenuItem( "Tall" ) )
                {
                }
                if ( ImGui::MenuItem( "Wide" ) )
                {
                }
                ImGui::EndMenu( );
            }
            ImGui::EndMenu( );
        }

        if ( ImGui::BeginMenu( "Help" ) )
        {
            if ( ImGui::MenuItem( "About" ) )
            {
            }
            ImGui::EndMenu( );
        }

        ImGui::EndMenuBar( );
    }
}

void Editor::RenderMainUI( uint32_t frameIndex )
{
    ImGuiWindowFlags     window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    const ImGuiViewport *viewport     = ImGui::GetMainViewport( );
    ImGui::SetNextWindowPos( viewport->Pos );
    ImGui::SetNextWindowSize( viewport->Size );
    ImGui::SetNextWindowViewport( viewport->ID );
    ImGui::PushStyleVar( ImGuiStyleVar_WindowRounding, 0.0f );
    ImGui::PushStyleVar( ImGuiStyleVar_WindowBorderSize, 0.0f );

    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 0.0f, 0.0f ) );
    ImGui::Begin( "DockSpace", nullptr, window_flags );
    ImGui::PopStyleVar( );
    ImGui::PopStyleVar( 2 );

    m_dockspaceId = ImGui::GetID( "MainDockSpace" );
    ImGui::DockSpace( m_dockspaceId, ImVec2( 0.0f, 0.0f ), ImGuiDockNodeFlags_None );

    if ( m_firstFrame )
    {
        SetupDefaultDockingLayout( );
        m_firstFrame = false;
    }

    RenderMenuBar( );
    if ( m_showDemoWindow )
    {
        ImGui::ShowDemoWindow( &m_showDemoWindow );
    }

    if ( m_assetBrowser )
    {
        m_assetBrowser->Render( );
    }

    if ( ImGui::Begin( "Inspector" ) )
    {
        ImGui::Text( "Inspector Panel" );
        ImGui::Text( "Selected object properties will appear here" );
    }
    ImGui::End( );

    if ( ImGui::Begin( "Hierarchy" ) )
    {
        ImGui::Text( "Scene Hierarchy" );
        ImGui::Text( "Scene objects will appear here" );
    }
    ImGui::End( );

    if ( ImGui::Begin( "Scene View" ) )
    {
        if ( const ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail( ); viewportPanelSize.x > 0 && viewportPanelSize.y > 0 && m_sceneViewRenderer )
        {
            Viewport sceneViewport{ };
            sceneViewport.X                 = 0;
            sceneViewport.Y                 = 0;
            sceneViewport.Width             = viewportPanelSize.x;
            sceneViewport.Height            = viewportPanelSize.y;
            ITextureResource *renderTexture = m_sceneViewRenderer->GetRenderTarget( frameIndex );
            if ( m_sceneViewTextureId == 0 )
            {
                m_sceneViewTextureId = m_imguiRenderer->AddTexture( renderTexture );
            }
            if ( m_sceneViewTextureId != 0 )
            {
                ImGui::Image( m_sceneViewTextureId, viewportPanelSize );
            }
        }
        else
        {
            ImGui::Text( "Scene View" );
            ImGui::Text( "3D scene viewport will appear here" );
        }
    }
    ImGui::End( );

    if ( ImGui::Begin( "Game View" ) )
    {
        if ( const ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail( ); viewportPanelSize.x > 0 && viewportPanelSize.y > 0 && frameIndex < m_gameRenderTargets.size( ) )
        {
            ITextureResource *renderTexture = m_gameRenderTargets[ frameIndex ].get( );
            if ( m_gameViewTextureId == 0 )
            {
                m_gameViewTextureId = m_imguiRenderer->AddTexture( renderTexture );
            }

            if ( m_gameViewTextureId != 0 )
            {
                ImGui::Image( m_gameViewTextureId, viewportPanelSize );
            }
        }
        else
        {
            ImGui::Text( "Game View" );
            ImGui::Text( "Game viewport will appear here" );
        }
    }
    ImGui::End( );
    ImGui::End( );
}

void Editor::SetupDefaultDockingLayout( ) const
{
    ImGui::DockBuilderRemoveNode( m_dockspaceId );
    ImGui::DockBuilderAddNode( m_dockspaceId, ImGuiDockNodeFlags_None );
    ImGui::DockBuilderSetNodeSize( m_dockspaceId, ImGui::GetMainViewport( )->Size );

    ImGuiID       dockMainId   = m_dockspaceId;
    const ImGuiID dockIdBottom = ImGui::DockBuilderSplitNode( dockMainId, ImGuiDir_Down, 0.25f, nullptr, &dockMainId );
    const ImGuiID dockIdLeft   = ImGui::DockBuilderSplitNode( dockMainId, ImGuiDir_Left, 0.2f, nullptr, &dockMainId );
    const ImGuiID dockIdRight  = ImGui::DockBuilderSplitNode( dockMainId, ImGuiDir_Right, 0.25f, nullptr, &dockMainId );
    const ImGuiID dockIdCenter = dockMainId;

    ImGui::DockBuilderDockWindow( "Asset Browser", dockIdBottom );
    ImGui::DockBuilderDockWindow( "Hierarchy", dockIdLeft );
    ImGui::DockBuilderDockWindow( "Inspector", dockIdRight );
    ImGui::DockBuilderDockWindow( "Scene View", dockIdCenter );
    ImGui::DockBuilderDockWindow( "Game View", dockIdCenter );
    ImGui::DockBuilderFinish( m_dockspaceId );
}

void Editor::CreateGameRenderTargets( )
{
    if ( m_gameViewport.Width <= 0 || m_gameViewport.Height <= 0 )
    {
        return;
    }

    m_gameRenderTargets.clear( );

    for ( uint32_t i = 0; i < m_graphicsContext->NumFramesInFlight; ++i )
    {
        TextureDesc renderTargetDesc{ };
        renderTargetDesc.Width      = static_cast<uint32_t>( m_gameViewport.Width );
        renderTargetDesc.Height     = static_cast<uint32_t>( m_gameViewport.Height );
        renderTargetDesc.Format     = Format::B8G8R8A8Unorm;
        renderTargetDesc.Descriptor = ResourceDescriptor::RenderTarget | ResourceDescriptor::Texture;
        renderTargetDesc.DebugName  = "GameViewRenderTarget";

        m_gameRenderTargets.push_back( std::unique_ptr<ITextureResource>( m_graphicsContext->LogicalDevice->CreateTextureResource( renderTargetDesc ) ) );

        m_graphicsContext->ResourceTracking->TrackTexture( m_gameRenderTargets[ i ].get( ), ResourceUsage::ShaderResource );
    }
}
