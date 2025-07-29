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
#include <memory>
#include <unordered_map>
#include <vector>
#include "DenOfIzGraphics/Backends/Common/ShaderProgram.h"
#include "DenOfIzGraphics/Backends/Interface/ICommandList.h"
#include "DenOfIzGraphics/Backends/Interface/ICommandListPool.h"
#include "DenOfIzGraphics/Backends/Interface/ICommandQueue.h"
#include "DenOfIzGraphics/Backends/Interface/IFence.h"
#include "DenOfIzGraphics/Backends/Interface/ILogicalDevice.h"
#include "DenOfIzGraphics/Backends/Interface/IPipeline.h"
#include "DenOfIzGraphics/Backends/Interface/IResourceBindGroup.h"
#include "DenOfIzGraphics/Backends/Interface/IRootSignature.h"
#include "DenOfIzGraphics/Input/Event.h"
#include "DenOfIzGraphics/UI/IClayContext.h"

#include <DirectXMath.h>

namespace DenOfIz
{
    struct ImGuiBackendDesc
    {
        ILogicalDevice *LogicalDevice      = nullptr;
        Format          RenderTargetFormat = Format::B8G8R8A8Unorm;
        uint32_t        NumFrames          = 3;
        uint32_t        MaxVertices        = 1310720;
        uint32_t        MaxIndices         = 1310720;
        uint32_t        MaxTextures        = 128;
        Viewport        Viewport{ };
    };

    class ImGuiBackend
    {
        struct ImGuiUniforms
        {
            Float_4x4 Projection;
            Float_4   ScreenSize;
        };

        struct PixelConstants
        {
            uint32_t TextureIndex;
        };

        ImGuiBackendDesc m_desc;
        ILogicalDevice  *m_logicalDevice = nullptr;

        std::unique_ptr<ShaderProgram>  m_shaderProgram{ };
        std::unique_ptr<IPipeline>      m_pipeline{ };
        std::unique_ptr<IRootSignature> m_rootSignature{ };
        std::unique_ptr<IInputLayout>   m_inputLayout{ };

        struct FrameData
        {
            std::unique_ptr<IResourceBindGroup> ConstantsBindGroup;
            std::unique_ptr<IResourceBindGroup> TextureBindGroup;
            ICommandList                       *CommandList = nullptr;
            std::unique_ptr<IFence>             FrameFence;
        };
        std::vector<FrameData> m_frameData{ };

        std::unique_ptr<ICommandQueue>    m_commandQueue{ };
        std::unique_ptr<ICommandListPool> m_commandListPool{ };

        std::unique_ptr<IBufferResource> m_vertexBuffer{ };
        std::unique_ptr<IBufferResource> m_indexBuffer{ };
        uint8_t                         *m_vertexBufferData = nullptr;
        uint8_t                         *m_indexBufferData  = nullptr;

        std::unique_ptr<IBufferResource> m_uniformBuffer{ };
        ImGuiUniforms                   *m_uniformBufferData  = nullptr;
        uint32_t                         m_alignedUniformSize = 0;

        std::unique_ptr<IBufferResource>                 m_pixelConstantsBuffer{ };
        PixelConstants                                  *m_pixelConstantsData        = nullptr;
        uint32_t                                         m_alignedPixelConstantsSize = 0;
        std::vector<std::unique_ptr<IResourceBindGroup>> m_pixelConstantsBindGroups{ };

        std::unordered_map<ImTextureID, uint32_t> m_textureToIndex{ };
        std::vector<ITextureResource *>           m_textures{ };
        std::unique_ptr<ITextureResource>         m_fontTexture{ };
        std::unique_ptr<ITextureResource>         m_nullTexture{ };
        bool                                      m_texturesDirty = true;

        Viewport  m_viewport;
        Float_4x4 m_projectionMatrix{ };

        std::unique_ptr<ISampler> m_linearSampler{ };
        uint32_t                  m_nextFrame    = 0;
        uint32_t                  m_currentFrame = 0;
        uint32_t                  m_numFrames    = 0;

    public:
        DZ_API explicit ImGuiBackend( const ImGuiBackendDesc &desc );
        DZ_API ~ImGuiBackend( );

        DZ_API void            SetViewport( const Viewport &viewport );
        DZ_API const Viewport &GetViewport( ) const;
        DZ_API void            NewFrame( ) const;
        DZ_API void            RenderDrawData( ICommandList *commandList, ImDrawData *drawData, uint32_t frameIndex );

        DZ_API void        RecreateFonts( );
        DZ_API ImTextureID AddTexture( ITextureResource *texture );
        DZ_API void        RemoveTexture( ImTextureID textureId );

        DZ_API void ProcessEvent( const Event &event ) const;

    private:
        void CreateShaderProgram( );
        void CreatePipeline( );
        void CreateBuffers( );
        void CreateNullTexture( );
        void CreateFontTexture( );

        void SetupRenderState( ICommandList *commandList, ImDrawData *drawData, uint32_t frameIndex ) const;
        void RenderImDrawList( ICommandList *commandList, ImDrawList *cmdList, uint32_t vertexOffset, uint32_t indexOffset ) const;
        void UpdateTextureBindings( );

        static ImGuiKey KeyCodeToImGuiKey( KeyCode keycode );
    };

    class DenOfIzImGuiContext
    {
        std::unique_ptr<ImGuiBackend> m_backend{ };

    public:
        DZ_API explicit DenOfIzImGuiContext( const ImGuiBackendDesc &desc );
        DZ_API ~DenOfIzImGuiContext( );

        DZ_API void ProcessEvent( const Event &event ) const;
        DZ_API void NewFrame( uint32_t width, uint32_t height, float deltaTime ) const;
        DZ_API void Render( ITextureResource *renderTarget, ICommandList *commandList, uint32_t frameIndex ) const;
        DZ_API void SetViewport( Viewport viewport ) const;
        DZ_API void RecreateFonts( ) const;

        DZ_API ImTextureID AddTexture( ITextureResource *texture ) const;
        DZ_API void        RemoveTexture( ImTextureID textureId ) const;
    };

    namespace ImGuiUtils
    {
        ImGuiBackend *CreateImGuiBackend( const ImGuiBackendDesc &desc );
        void          DestroyImGuiBackend( const ImGuiBackend *backend );
        void          InitializeImGui( ImGuiBackend *backend );
        void          ShutdownImGui( );
    } // namespace ImGuiUtils

} // namespace DenOfIz
