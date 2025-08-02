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

#include <cstdint>
#include "DZEngine/AssetHandle.h"

namespace DZEngine
{
    struct AnimationComponent
    {
        AnimationClipHandle AnimationClip;
        uint32_t            AnimationStateId = 0; // References state in animation system
        float               PlaybackSpeed    = 1.0f;
        float               CurrentTime      = 0.0f;
        bool                IsPlaying        = false;
        bool                Loop             = true;
    };
} // namespace DZEngine
