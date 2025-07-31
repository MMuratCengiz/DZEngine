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

#include "DZEngine/Style/EditorStyle.h"
#include <imgui_internal.h>

using namespace DZEngine;

void EditorStyle::ApplyTheme( const Theme theme )
{
    switch ( theme )
    {
    case Theme::Default:
        ApplyDefaultStyle( );
        break;
    case Theme::Dark:
        ApplyDarkStyle( );
        break;
    case Theme::Light:
        ApplyLightStyle( );
        break;
    }
}

void EditorStyle::ApplyDefaultStyle( )
{
    SetupColors( Colors::DARK_BG, Colors::DARK_TEXT, Colors::DARK_BORDER, Colors::BLUE, Colors::BLUE_HOVER, Colors::BLUE_ACTIVE );
    SetupSizes( );

    ImGuiStyle &style       = ImGui::GetStyle( );
    style.WindowPadding     = ImVec2( 8, 8 );
    style.FramePadding      = ImVec2( 4, 3 );
    style.ItemSpacing       = ImVec2( 8, 4 );
    style.ItemInnerSpacing  = ImVec2( 4, 4 );
    style.TouchExtraPadding = ImVec2( 0, 0 );
    style.IndentSpacing     = 21;
    style.ScrollbarSize     = 14;
    style.GrabMinSize       = 10;

    style.WindowBorderSize = 1;
    style.ChildBorderSize  = 1;
    style.PopupBorderSize  = 1;
    style.FrameBorderSize  = 1;
    style.TabBorderSize    = 0;

    style.WindowRounding    = 0;
    style.ChildRounding     = 0;
    style.FrameRounding     = 0;
    style.PopupRounding     = 0;
    style.ScrollbarRounding = 0;
    style.GrabRounding      = 0;
    style.TabRounding       = 0;

    style.WindowTitleAlign    = ImVec2( 0.5f, 0.5f );
    style.ButtonTextAlign     = ImVec2( 0.5f, 0.5f );
    style.SelectableTextAlign = ImVec2( 0.0f, 0.0f );

    ImVec4 *colors                           = style.Colors;
    colors[ ImGuiCol_WindowBg ]              = Colors::DARK_PANEL;
    colors[ ImGuiCol_ChildBg ]               = Colors::DARK_PANEL;
    colors[ ImGuiCol_PopupBg ]               = Colors::DARK_PANEL;
    colors[ ImGuiCol_FrameBg ]               = ImVec4( 0.12f, 0.12f, 0.12f, 1.0f );
    colors[ ImGuiCol_FrameBgHovered ]        = ImVec4( 0.18f, 0.18f, 0.18f, 1.0f );
    colors[ ImGuiCol_FrameBgActive ]         = ImVec4( 0.24f, 0.24f, 0.24f, 1.0f );
    colors[ ImGuiCol_TitleBg ]               = Colors::DARK_HEADER;
    colors[ ImGuiCol_TitleBgActive ]         = Colors::DARK_HEADER;
    colors[ ImGuiCol_TitleBgCollapsed ]      = Colors::DARK_HEADER;
    colors[ ImGuiCol_MenuBarBg ]             = Colors::DARK_HEADER;
    colors[ ImGuiCol_ScrollbarBg ]           = ImVec4( 0.12f, 0.12f, 0.12f, 1.0f );
    colors[ ImGuiCol_ScrollbarGrab ]         = ImVec4( 0.3f, 0.3f, 0.3f, 1.0f );
    colors[ ImGuiCol_ScrollbarGrabHovered ]  = ImVec4( 0.4f, 0.4f, 0.4f, 1.0f );
    colors[ ImGuiCol_ScrollbarGrabActive ]   = ImVec4( 0.5f, 0.5f, 0.5f, 1.0f );
    colors[ ImGuiCol_CheckMark ]             = Colors::BLUE;
    colors[ ImGuiCol_SliderGrab ]            = Colors::BLUE;
    colors[ ImGuiCol_SliderGrabActive ]      = Colors::BLUE_ACTIVE;
    colors[ ImGuiCol_Button ]                = ImVec4( 0.2f, 0.2f, 0.2f, 1.0f );
    colors[ ImGuiCol_ButtonHovered ]         = ImVec4( 0.3f, 0.3f, 0.3f, 1.0f );
    colors[ ImGuiCol_ButtonActive ]          = ImVec4( 0.15f, 0.15f, 0.15f, 1.0f );
    colors[ ImGuiCol_Header ]                = ImVec4( 0.2f, 0.2f, 0.2f, 1.0f );
    colors[ ImGuiCol_HeaderHovered ]         = ImVec4( 0.3f, 0.3f, 0.3f, 1.0f );
    colors[ ImGuiCol_HeaderActive ]          = ImVec4( 0.15f, 0.15f, 0.15f, 1.0f );
    colors[ ImGuiCol_Separator ]             = Colors::DARK_BORDER;
    colors[ ImGuiCol_SeparatorHovered ]      = ImVec4( 0.4f, 0.4f, 0.4f, 1.0f );
    colors[ ImGuiCol_SeparatorActive ]       = ImVec4( 0.4f, 0.4f, 0.4f, 1.0f );
    colors[ ImGuiCol_ResizeGrip ]            = ImVec4( 0.2f, 0.2f, 0.2f, 1.0f );
    colors[ ImGuiCol_ResizeGripHovered ]     = ImVec4( 0.3f, 0.3f, 0.3f, 1.0f );
    colors[ ImGuiCol_ResizeGripActive ]      = ImVec4( 0.15f, 0.15f, 0.15f, 1.0f );
    colors[ ImGuiCol_Tab ]                   = ImVec4( 0.18f, 0.18f, 0.18f, 1.0f );
    colors[ ImGuiCol_TabHovered ]            = ImVec4( 0.3f, 0.3f, 0.3f, 1.0f );
    colors[ ImGuiCol_TabActive ]             = ImVec4( 0.24f, 0.24f, 0.24f, 1.0f );
    colors[ ImGuiCol_TabUnfocused ]          = ImVec4( 0.15f, 0.15f, 0.15f, 1.0f );
    colors[ ImGuiCol_TabUnfocusedActive ]    = ImVec4( 0.20f, 0.20f, 0.20f, 1.0f );
    colors[ ImGuiCol_DockingPreview ]        = Colors::BLUE;
    colors[ ImGuiCol_DockingEmptyBg ]        = Colors::DARK_BG;
    colors[ ImGuiCol_PlotLines ]             = Colors::BLUE;
    colors[ ImGuiCol_PlotLinesHovered ]      = Colors::BLUE_HOVER;
    colors[ ImGuiCol_PlotHistogram ]         = Colors::BLUE;
    colors[ ImGuiCol_PlotHistogramHovered ]  = Colors::BLUE_HOVER;
    colors[ ImGuiCol_TextSelectedBg ]        = ImVec4( 0.3f, 0.5f, 0.8f, 0.35f );
    colors[ ImGuiCol_DragDropTarget ]        = Colors::BLUE;
    colors[ ImGuiCol_NavHighlight ]          = Colors::BLUE;
    colors[ ImGuiCol_NavWindowingHighlight ] = ImVec4( 1.0f, 1.0f, 1.0f, 0.7f );
    colors[ ImGuiCol_NavWindowingDimBg ]     = ImVec4( 0.8f, 0.8f, 0.9f, 0.2f );
    colors[ ImGuiCol_ModalWindowDimBg ]      = ImVec4( 0.0f, 0.0f, 0.0f, 0.6f );
}

void EditorStyle::ApplyDarkStyle( )
{
    ImGui::StyleColorsDark( );
    SetupSizes( );
}

void EditorStyle::ApplyLightStyle( )
{
    ImGui::StyleColorsLight( );
    SetupSizes( );
}

void EditorStyle::SetupColors( const ImVec4 &background, const ImVec4 &text, const ImVec4 &border, const ImVec4 &button, const ImVec4 &buttonHovered, const ImVec4 &buttonActive )
{
    ImGuiStyle &style                      = ImGui::GetStyle( );
    style.Colors[ ImGuiCol_WindowBg ]      = background;
    style.Colors[ ImGuiCol_Text ]          = text;
    style.Colors[ ImGuiCol_Border ]        = border;
    style.Colors[ ImGuiCol_Button ]        = button;
    style.Colors[ ImGuiCol_ButtonHovered ] = buttonHovered;
    style.Colors[ ImGuiCol_ButtonActive ]  = buttonActive;
}

void EditorStyle::SetupSizes( )
{
    ImGuiStyle &style      = ImGui::GetStyle( );
    style.WindowPadding    = ImVec2( 8, 8 );
    style.FramePadding     = ImVec2( 4, 3 );
    style.ItemSpacing      = ImVec2( 8, 4 );
    style.ItemInnerSpacing = ImVec2( 4, 4 );
    style.IndentSpacing    = 21;
    style.ScrollbarSize    = 14;
    style.GrabMinSize      = 10;
}

void EditorStyle::PushItemSpacing( const float x, const float y )
{
    ImGui::PushStyleVar( ImGuiStyleVar_ItemSpacing, ImVec2( x, y ) );
}

void EditorStyle::PopItemSpacing( )
{
    ImGui::PopStyleVar( );
}

void EditorStyle::PushFramePadding( const float x, const float y )
{
    ImGui::PushStyleVar( ImGuiStyleVar_FramePadding, ImVec2( x, y ) );
}

void EditorStyle::PopFramePadding( )
{
    ImGui::PopStyleVar( );
}

void EditorStyle::BeginToolbar( )
{
    ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 4, 4 ) );
    ImGui::PushStyleVar( ImGuiStyleVar_ItemSpacing, ImVec2( 4, 4 ) );
    ImGui::PushStyleColor( ImGuiCol_Button, ImVec4( 0, 0, 0, 0 ) );
    ImGui::PushStyleColor( ImGuiCol_ButtonHovered, ImVec4( 0.3f, 0.3f, 0.3f, 0.5f ) );
    ImGui::PushStyleColor( ImGuiCol_ButtonActive, ImVec4( 0.2f, 0.2f, 0.2f, 0.5f ) );
}

void EditorStyle::EndToolbar( )
{
    ImGui::PopStyleColor( 3 );
    ImGui::PopStyleVar( 2 );
}

void EditorStyle::BeginDockSpace( )
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
    ImGui::Begin( "DockSpace Demo", nullptr, window_flags );
    ImGui::PopStyleVar( );
    ImGui::PopStyleVar( 2 );

    const ImGuiID dockSpaceId = ImGui::GetID( "MyDockSpace" );
    ImGui::DockSpace( dockSpaceId, ImVec2( 0.0f, 0.0f ), ImGuiDockNodeFlags_None );
}

void EditorStyle::EndDockSpace( )
{
    ImGui::End( );
}
