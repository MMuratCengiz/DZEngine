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

#include <DenOfIzGraphics/DenOfIzGraphics.h>
#include "AnimationAssetData.h"
#include "DZEngine/Components/AssetHandle.h"

namespace DZEngine
{
    struct AnimationBatchDesc
    {
        ILogicalDevice *LogicalDevice;
    };

    class AnimationBatch
    {
        ILogicalDevice                    *m_logicalDevice;
        std::unique_ptr<BatchResourceCopy> m_batchResourceCopy;

        std::vector<std::unique_ptr<AnimationAssetData>>     m_animationData;
        std::unordered_map<std::string, AnimationClipHandle> m_animAliases;

        std::mutex m_nextAnimHandleLock;
        size_t     m_nextAnimHandle = 0;

    public:
        explicit AnimationBatch( const AnimationBatchDesc &desc );
        ~AnimationBatch( ) = default;

        void BeginUpdate( );
        void EndUpdate( ISemaphore *onComplete = nullptr ); // nullptr will block execution

        AnimationClipHandle LoadAnimation( const std::string &alias, BinaryReader &reader );
        AnimationClipHandle AddAnimation( const std::string &alias, const AnimationAssetData &animationData );

        AnimationAssetData *GetAnimation( const std::string &alias );
        AnimationAssetData *GetAnimation( AnimationClipHandle handle ) const;

    private:
        size_t NextAnimationHandle( const std::string &alias );
    };
} // namespace DZEngine
