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

#include <memory>
#include "DenOfIzGraphics/DenOfIzGraphics.h"

using namespace DenOfIz;

namespace DZEngine
{
    struct SceneViewRendererDesc
    {
        ILogicalDevice *LogicalDevice = nullptr;
        Viewport        Viewport{ };
        uint32_t        NumFramesInFlight = 3;
    };

    class SceneViewRenderer
    {
        ILogicalDevice *m_logicalDevice = nullptr;
        Viewport        m_viewport{ };
        uint32_t        m_numFramesInFlight = 3;

        std::unique_ptr<IBufferResource> m_vertexBuffer;
        std::unique_ptr<ShaderProgram>   m_shaderProgram;
        std::unique_ptr<IInputLayout>    m_inputLayout;
        std::unique_ptr<IRootSignature>  m_rootSignature;
        std::unique_ptr<IPipeline>       m_pipeline;

        std::vector<std::unique_ptr<ITextureResource>> m_renderTargets;
        std::vector<std::unique_ptr<ITextureResource>> m_depthTextures;

        ResourceTracking m_resourceTracking{ };

    public:
        explicit SceneViewRenderer( const SceneViewRendererDesc &desc );
        ~SceneViewRenderer( ) = default;

        void              UpdateViewport( const Viewport &viewport );
        void              Render( ICommandList *commandList, uint32_t frameIndex );
        ITextureResource *GetRenderTarget( uint32_t frameIndex ) const;

    private:
        void      CreateVertexBuffer( );
        void      CreateShaderProgram( );
        void      CreatePipeline( );
        void      CreateRenderTargets( );
        ByteArray GetVertexShader( ) const;
        ByteArray GetPixelShader( ) const;
    };
} // namespace DZEngine
