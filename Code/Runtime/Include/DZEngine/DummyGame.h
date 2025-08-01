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
#include "AppContext.h"
#include "DenOfIzGraphics/DenOfIzGraphics.h"
#include "IGame.h"

using namespace DenOfIz;

namespace DZEngine
{
    class DummyGame final : public IGame
    {
        AppContext       *m_appContext       = nullptr;
        ILogicalDevice   *m_logicalDevice    = nullptr;
        ResourceTracking *m_resourceTracking = nullptr;

        std::unique_ptr<IBufferResource> m_vertexBuffer;
        std::unique_ptr<ShaderProgram>   m_shaderProgram;
        std::unique_ptr<IInputLayout>    m_inputLayout;
        std::unique_ptr<IRootSignature>  m_rootSignature;
        std::unique_ptr<IPipeline>       m_pipeline;

        std::unique_ptr<ICommandListPool> m_commandListPool;
        std::vector<ICommandList *>       m_commandLists;

    public:
        ~DummyGame( ) override;
        void Init( AppContext *appContext ) override;
        void HandleEvent( const Event &event ) override;
        void Update( ) override;
        bool Render( RenderDesc renderDesc ) override;

    private:
        void      CreateVertexBuffer( );
        void      CreateShaderProgram( );
        void      CreatePipeline( );
        ByteArray GetVertexShader( ) const;
        ByteArray GetPixelShader( ) const;
    };
} // namespace DZEngine
