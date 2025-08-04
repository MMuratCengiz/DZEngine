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

#include "DZEngine/Rendering/IRenderer.h"
#include "GPUDrivenDataUpload.h"
#include "GPUDrivenRootSig.h"

namespace DZEngine
{
    struct GPUDrivenBindingDesc
    {
        GPUDrivenRootSig    *RootSig;
        GPUDrivenDataUpload *DataUpload;
        AppContext          *AppContext;
        uint32_t             BatchId;
    };

    class GPUDrivenBinding
    {
        static constexpr uint32_t TexturesSpace = 0;
        static constexpr uint32_t BuffersSpace  = 1;
        static constexpr uint32_t SamplerSpace  = 2;

        GraphicsContext     *m_graphicsContext;
        uint32_t             m_numFrames;
        GPUDrivenRootSig    *m_rootSig;
        GPUDrivenDataUpload *m_dataUpload;
        AssetBatcher        *m_assetBatcher;
        World               *m_world;
        uint32_t             m_batchId;

        struct FrameBinding
        {
            std::unique_ptr<IResourceBindGroup> BuffersBinding;
            std::unique_ptr<IResourceBindGroup> TexturesBinding;
        };
        std::unique_ptr<ISampler>         m_linearSampler;
        std::unique_ptr<ISampler>         m_pointSampler;
        std::unique_ptr<ISampler>         m_anisotropicSampler;
        std::unique_ptr<ITextureResource> m_nullTexture;

        std::unique_ptr<IResourceBindGroup> m_samplerBindGroup;

        std::vector<std::unique_ptr<FrameBinding>> m_frameBindings;

    public:
        explicit GPUDrivenBinding( const GPUDrivenBindingDesc &bindingDesc );
        void Update( const uint32_t frameIndex ) const;

        IResourceBindGroup *GetSamplerBinding( ) const;
        IResourceBindGroup *GetBuffersBinding( uint32_t frameIndex ) const;
        IResourceBindGroup *GetTexturesBinding( uint32_t frameIndex ) const;

        ~GPUDrivenBinding( ) = default;

    private:
        void CreateSamplersBinding( );
        void CreateBuffersBinding( ) const;
        void CreateTexturesBinding( );
        void UpdateTextures( uint32_t frameIndex ) const;
    };
} // namespace DZEngine
