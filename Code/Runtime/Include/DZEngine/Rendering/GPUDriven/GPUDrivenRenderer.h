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

#include "../IRenderer.h"
#include "GPUDrivenBinding.h"
#include "GPUDrivenDataUpload.h"
#include "GPUDrivenRootSig.h"

namespace DZEngine
{
    class GPUDrivenRenderer final : public IRenderer
    {
        GraphicsContext                  *m_graphicsContext;
        uint32_t                          m_numFrames;
        std::unique_ptr<GPUDrivenRootSig> m_rootSig;
        AssetBatcher                     *m_assetBatcher;
        World                            *m_world;

        struct BatchData
        {
            std::unique_ptr<GPUDrivenDataUpload> DataUpload;
            std::unique_ptr<GPUDrivenBinding>    DataBinding;
        };

        std::vector<std::unique_ptr<BatchData>> m_batches;

        // TODO temporary for testing
        std::vector<std::unique_ptr<ISemaphore>> m_signalSemaphores;
        std::unique_ptr<ICommandQueue>           m_commandQueue;
        std::unique_ptr<ICommandListPool>        m_commandListPool;
        std::vector<ICommandList *>              m_commandLists;
        std::unique_ptr<ShaderProgram>           m_program;
        std::unique_ptr<IPipeline>               m_pipeline;

    public:
        explicit GPUDrivenRenderer( const RendererDesc &rendererDesc );
        ISemaphore *RenderFrame( const RenderFrameDesc &renderFrame ) override;
        void        InitTestPipeline( ); // Todo use render graph here and more dynamic pipelines
        ~GPUDrivenRenderer( ) override = default;
    };
} // namespace DZEngine
