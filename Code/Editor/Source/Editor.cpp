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

extern unsigned int  JetbrainsMonoImGui_size;
extern unsigned char JetbrainsMonoImGui_data[];

extern unsigned int  FontAwesome_size;
extern unsigned char FontAwesome_data[];

Editor::Editor( const EditorDesc editorDesc ) : m_appContext( editorDesc.AppContext ), m_graphicsContext( editorDesc.AppContext->GraphicsContext )
{
    ImGuiBackendDesc backendDesc{ };
    backendDesc.LogicalDevice      = m_graphicsContext->LogicalDevice;
    backendDesc.RenderTargetFormat = Format::B8G8R8A8Unorm;
    backendDesc.NumFrames          = m_graphicsContext->NumFramesInFlight;
    backendDesc.Viewport           = m_graphicsContext->SwapChain->GetViewport( );
    ImGuiRenderer renderer( backendDesc );
    m_imguiRenderer                = std::make_unique<ImGuiRenderer>( backendDesc );

    ImGuiIO &io = ImGui::GetIO( );
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    io.Fonts->Clear( );

    ImFontConfig fontConfig;
    fontConfig.FontDataOwnedByAtlas = false;
    m_font                          = io.Fonts->AddFontFromMemoryTTF( JetbrainsMonoImGui_data, JetbrainsMonoImGui_size, 24.0f, &fontConfig );

    fontConfig.MergeMode                   = true;
    fontConfig.GlyphMinAdvanceX            = 24.0f;
    static constexpr ImWchar icon_ranges[] = { 0xf000, 0xf8ff, 0 };
    io.Fonts->AddFontFromMemoryTTF( FontAwesome_data, FontAwesome_size, 24.0f, &fontConfig, icon_ranges );
    io.Fonts->Build( );
    m_imguiRenderer->RecreateFonts( );

    CommandListPoolDesc commandListPoolDesc{ };
    commandListPoolDesc.CommandQueue    = m_graphicsContext->GraphicsQueue;
    commandListPoolDesc.NumCommandLists = m_graphicsContext->NumFramesInFlight;
    m_commandListPool                   = std::unique_ptr<ICommandListPool>( m_graphicsContext->LogicalDevice->CreateCommandListPool( commandListPoolDesc ) );

    const ICommandListArray commandLists = m_commandListPool->GetCommandLists( );
    m_commandLists.resize( commandLists.NumElements );
    for ( int i = 0; i < commandLists.NumElements; ++i )
    {
        m_commandLists[ i ] = commandLists.Elements[ i ];
    }
}

Editor::~Editor( )
{
}

GameRenderView Editor::GetGameRenderView( uint32_t frameIndex )
{
    GameRenderView renderView{ };

    return renderView;
}

void Editor::HandleEvent( const Event &event ) const
{
    m_imguiRenderer->ProcessEvent( event );
}

void Editor::Update( EditorUpdateDesc updateDesc )
{
    m_stepTimer.Tick( );

    ICommandList *commandList = m_commandLists[ updateDesc.FrameIndex ];

    commandList->Begin( );

    ITextureResource *renderTarget = updateDesc.RenderTarget;
    m_graphicsContext->ResourceTracking->TransitionTexture( commandList, renderTarget, ResourceUsage::RenderTarget );

    const auto viewport = m_graphicsContext->SwapChain->GetViewport( );
    m_imguiRenderer->NewFrame( static_cast<uint32_t>( viewport.Width ), static_cast<uint32_t>( viewport.Height ), m_stepTimer.GetDeltaTime( ) );

    ImGui::ShowDemoWindow( );

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
    if ( updateDesc.GameSemaphore )
    {
        executeCommandListsDesc.WaitSemaphores.Elements    = &updateDesc.GameSemaphore;
        executeCommandListsDesc.WaitSemaphores.NumElements = 1;
    }
    m_graphicsContext->GraphicsQueue->ExecuteCommandLists( executeCommandListsDesc );
}
