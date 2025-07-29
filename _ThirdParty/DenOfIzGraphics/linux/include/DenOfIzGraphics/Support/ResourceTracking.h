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

#include <mutex>
#include <unordered_map>
#include "DenOfIzGraphics/Backends/Interface/ILogicalDevice.h"

namespace DenOfIz
{
    struct ResourceState
    {
        std::mutex Mutex;
        uint32_t   CurrentUsage = ResourceUsage::Undefined;
        QueueType  CurrentQueue = QueueType::Graphics;
    };

    struct DZ_API TransitionBufferDesc
    {
        IBufferResource *Buffer    = nullptr;
        uint32_t         NewUsage  = ResourceUsage::Undefined;
        QueueType        QueueType = QueueType::Graphics;
    };

    struct DZ_API TransitionTextureDesc
    {
        ITextureResource *Texture   = nullptr;
        uint32_t          NewUsage  = ResourceUsage::Undefined;
        QueueType         QueueType = QueueType::Graphics;
    };

    struct DZ_API BatchTransitionDesc
    {
        TransitionBufferDesc  *Buffers     = nullptr;
        size_t                 NumBuffers  = 0;
        TransitionTextureDesc *Textures    = nullptr;
        size_t                 NumTextures = 0;
    };

    class ResourceTracking
    {
        std::unordered_map<IBufferResource *, ResourceState>  m_bufferStates;
        std::unordered_map<ITextureResource *, ResourceState> m_textureStates;

    public:
        DZ_API ResourceTracking( ) = default;
        DZ_API ~ResourceTracking( );

        DZ_API void TrackBuffer( IBufferResource *buffer, const uint32_t &currentUsage, QueueType queueType = QueueType::Graphics );
        DZ_API void TrackTexture( ITextureResource *texture, const uint32_t &currentUsage, QueueType queueType = QueueType::Graphics );

        DZ_API void UntrackBuffer( IBufferResource *buffer );
        DZ_API void UntrackTexture( ITextureResource *texture );

        DZ_API void TransitionBuffer( ICommandList *commandList, IBufferResource *buffer, const uint32_t &newUsage, QueueType queueType = QueueType::Graphics );
        DZ_API void TransitionTexture( ICommandList *commandList, ITextureResource *texture, const uint32_t &newUsage, QueueType queueType = QueueType::Graphics );
        DZ_API void BatchTransition( ICommandList *commandList, const BatchTransitionDesc &desc );

    private:
        void ProcessBufferTransitions( const TransitionBufferDesc *buffers, size_t numBuffers, PipelineBarrierDesc &barrier );
        void ProcessTextureTransitions( const TransitionTextureDesc *textures, size_t numTextures, PipelineBarrierDesc &barrier );
    };
} // namespace DenOfIz
