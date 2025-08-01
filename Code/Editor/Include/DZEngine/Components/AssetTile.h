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

#include <imgui.h>
#include <string>
#include "DZEngine/Style/ImGuiFonts.h"

namespace DZEngine
{
    // Forward declaration - AssetItem is defined in AssetBrowser.h
    struct AssetItem;

    class AssetTile
    {
    public:
        struct Style
        {
            ImVec4 selectedColor        = ImVec4(0.3f, 0.5f, 0.8f, 0.4f);
            ImVec4 hoveredColor         = ImVec4(0.3f, 0.3f, 0.3f, 0.3f);
            ImVec4 normalColor          = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
            ImVec4 textColor            = ImVec4(0.9f, 0.9f, 0.9f, 1.0f);
            float  iconAreaRatio        = 0.65f;  // Reduced from 0.7f to give more space for text
            float  textAreaRatio        = 0.35f;  // Increased from 0.3f to give more space for text
            float  padding              = 4.0f;
            float  textPadding          = 2.0f;
            float  borderRadius         = 4.0f;
            FontSize textFontSize       = FontSize::VerySmall;
        };

        static bool Render(const AssetItem& item, int tileSize, bool isSelected, int uniqueId, const Style& style = Style{});
        
    private:
        static const char* GetFileIcon(const AssetItem& item);
        static ImVec4 GetFileIconColor(const AssetItem& item);
        static FontSize GetIconFontSize(int tileSize);
    };
} // namespace DZEngine