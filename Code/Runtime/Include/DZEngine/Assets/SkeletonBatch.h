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
#include "DZEngine/Components/AssetHandle.h"
#include "SkeletonAssetData.h"

namespace DZEngine
{
    struct SkeletonBatchDesc
    {
        ILogicalDevice *LogicalDevice;
    };

    class SkeletonBatch
    {
        ILogicalDevice                    *m_logicalDevice;
        std::unique_ptr<BatchResourceCopy> m_batchResourceCopy;

        std::vector<std::unique_ptr<SkeletonAssetData>> m_skeletonData;
        std::unordered_map<std::string, SkeletonHandle> m_skelAliases;

        std::mutex m_nextSkelHandleLock;
        size_t     m_nextSkelHandle = 0;

    public:
        explicit SkeletonBatch( const SkeletonBatchDesc &desc );
        ~SkeletonBatch( ) = default;

        void BeginUpdate( );
        void EndUpdate( ISemaphore *onComplete = nullptr ); // nullptr will block execution

        SkeletonHandle LoadSkeleton( const std::string &alias, BinaryReader &reader );
        SkeletonHandle AddSkeleton( const std::string &alias, const SkeletonAssetData &skeletonData );

        SkeletonAssetData *GetSkeleton( const std::string &alias );
        SkeletonAssetData *GetSkeleton( SkeletonHandle handle ) const;

    private:
        size_t NextSkeletonHandle( const std::string &alias );
    };
} // namespace DZEngine
