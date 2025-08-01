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

#include <array>
#include "DenOfIzGraphics/Utilities/Common_Arrays.h"
#include "IBufferResource.h"
#include "IFence.h"
#include "IPipeline.h"
#include "IResourceBindGroup.h"
#include "ITextureResource.h"
#include "PipelineBarrierDesc.h"
#include "RayTracing/IBottomLevelAS.h"
#include "RayTracing/IShaderBindingTable.h"
#include "RayTracing/ITopLevelAS.h"

namespace DenOfIz
{

    // Swig Note: ClearColor/ClearDepth are ignored, use SetClearColor/SetClearDepth instead.
    struct DZ_API RenderingAttachmentDesc
    {
        LoadOp  LoadOp  = LoadOp::Clear;
        StoreOp StoreOp = StoreOp::Store;

        ITextureResource *Resource = nullptr;

        float ClearColor[ 4 ]{ 0.0f, 0.0f, 0.0f, 1.0f };
        float ClearDepthStencil[ 2 ]{ 1.0f, 0.0f };

        void SetClearColor( float r, float g, float b, float a )
        {
            ClearColor[ 0 ] = r;
            ClearColor[ 1 ] = g;
            ClearColor[ 2 ] = b;
            ClearColor[ 3 ] = a;
        }

        void SetClearDepthStencil( float depth, float stencil )
        {
            ClearDepthStencil[ 0 ] = depth;
            ClearDepthStencil[ 1 ] = stencil;
        }
    };

    struct DZ_API RenderingAttachmentDescArray
    {
        RenderingAttachmentDesc *Elements    = nullptr;
        uint32_t                 NumElements = 0;
    };

    struct DZ_API RenderingDesc
    {
        RenderingAttachmentDesc DepthAttachment;
        RenderingAttachmentDesc StencilAttachment;

        float    RenderAreaWidth   = 0.0f;
        float    RenderAreaHeight  = 0.0f;
        float    RenderAreaOffsetX = 0.0f;
        float    RenderAreaOffsetY = 0.0f;
        uint32_t LayerCount        = 1;

        RenderingAttachmentDescArray RTAttachments;
    };

    struct DZ_API CopyBufferRegionDesc
    {
        IBufferResource *DstBuffer = nullptr;
        uint64_t         DstOffset = 0;
        IBufferResource *SrcBuffer = nullptr;
        uint64_t         SrcOffset = 0;
        uint64_t         NumBytes  = 0;
    };

    struct DZ_API CopyTextureRegionDesc
    {
        ITextureResource *SrcTexture    = nullptr;
        ITextureResource *DstTexture    = nullptr;
        uint32_t          SrcX          = 0;
        uint32_t          SrcY          = 0;
        uint32_t          SrcZ          = 0;
        uint32_t          DstX          = 0;
        uint32_t          DstY          = 0;
        uint32_t          DstZ          = 0;
        uint32_t          Width         = 0;
        uint32_t          Height        = 0;
        uint32_t          Depth         = 0;
        uint32_t          SrcMipLevel   = 0;
        uint32_t          DstMipLevel   = 0;
        uint32_t          SrcArrayLayer = 0;
        uint32_t          DstArrayLayer = 0;
    };

    struct DZ_API CopyBufferToTextureDesc
    {
        ITextureResource *DstTexture = nullptr;
        IBufferResource  *SrcBuffer  = nullptr;
        size_t            SrcOffset  = 0;
        uint32_t          DstX       = 0;
        uint32_t          DstY       = 0;
        uint32_t          DstZ       = 0;
        Format            Format     = Format::R8G8B8A8Unorm;
        uint32_t          MipLevel   = 0;
        uint32_t          ArrayLayer = 0;
        // Information below is optional, 0 tries to calculate it automatically, sometimes it may be needed to be set manually
        uint32_t RowPitch = 0;
        uint32_t NumRows  = 0;
    };

    struct DZ_API CopyTextureToBufferDesc
    {
        IBufferResource  *DstBuffer  = nullptr;
        ITextureResource *SrcTexture = nullptr;
        uint32_t          DstOffset  = 0;
        uint32_t          SrcX       = 0;
        uint32_t          SrcY       = 0;
        uint32_t          SrcZ       = 0;
        Format            Format     = Format::R8G8B8A8Unorm;
        uint32_t          MipLevel   = 0;
        uint32_t          ArrayLayer = 0;
        // Information below is optional, 0 tries to calculate it automatically, sometimes it may be needed to be set manually
        uint32_t RowPitch = 0;
        uint32_t NumRows  = 0;
    };

    struct DZ_API DispatchRaysDesc
    {
        uint32_t             Width              = 1;
        uint32_t             Height             = 1;
        uint32_t             Depth              = 1;
        IShaderBindingTable *ShaderBindingTable = nullptr;
    };

    struct DZ_API DrawIndexedIndirectCommand
    {
        uint32_t NumIndices;
        uint32_t NumInstances;
        uint32_t FirstIndex;
        int32_t  VertexOffset;
        uint32_t FirstInstance;
    };

    struct DZ_API CommandListDesc
    {
        QueueType QueueType = QueueType::Graphics;
    };

    struct DZ_API BuildTopLevelASDesc
    {
        ITopLevelAS *TopLevelAS = nullptr;
    };

    struct DZ_API UpdateTopLevelASDesc
    {
        ITopLevelAS    *TopLevelAS = nullptr;
        FloatArrayArray Transforms;
    };

    struct DZ_API BuildBottomLevelASDesc
    {
        IBottomLevelAS *BottomLevelAS = nullptr;
    };

    class DZ_API ICommandList
    {
    public:
        virtual ~ICommandList( ) = default;

        virtual void Begin( )                                                                                                                                   = 0;
        virtual void BeginRendering( const RenderingDesc &renderingDesc )                                                                                       = 0;
        virtual void EndRendering( )                                                                                                                            = 0;
        virtual void End( )                                                                                                                                     = 0;
        virtual void BindPipeline( IPipeline *pipeline )                                                                                                        = 0;
        virtual void BindVertexBuffer( IBufferResource *buffer, uint64_t offset = 0, uint32_t stride = 0, uint32_t slot = 0 )                                   = 0;
        virtual void BindIndexBuffer( IBufferResource *buffer, const IndexType &indexType, uint64_t offset = 0 )                                                = 0;
        virtual void BindViewport( float x, float y, float width, float height )                                                                                = 0;
        virtual void BindScissorRect( float x, float y, float width, float height )                                                                             = 0;
        virtual void BindResourceGroup( IResourceBindGroup *bindGroup )                                                                                         = 0;
        virtual void PipelineBarrier( const PipelineBarrierDesc &barrier )                                                                                      = 0;
        virtual void DrawIndexed( uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex = 0, uint32_t vertexOffset = 0, uint32_t firstInstance = 0 ) = 0;
        virtual void Draw( uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex = 0, uint32_t firstInstance = 0 )                                 = 0;
        // List of copy commands
        virtual void CopyBufferRegion( const CopyBufferRegionDesc &copyBufferRegionDesc )      = 0;
        virtual void CopyTextureRegion( const CopyTextureRegionDesc &copyTextureRegionDesc )   = 0;
        virtual void CopyBufferToTexture( const CopyBufferToTextureDesc &copyBufferToTexture ) = 0;
        virtual void CopyTextureToBuffer( const CopyTextureToBufferDesc &copyTextureToBuffer ) = 0;
        // --
        virtual void UpdateTopLevelAS( const UpdateTopLevelASDesc &updateDesc )                                           = 0;
        virtual void BuildTopLevelAS( const BuildTopLevelASDesc &buildTopLevelASDesc )                                    = 0;
        virtual void BuildBottomLevelAS( const BuildBottomLevelASDesc &buildBottomLevelASDesc )                           = 0;
        virtual void DispatchRays( const DispatchRaysDesc &dispatchRaysDesc )                                             = 0;
        virtual void Dispatch( uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ )                         = 0;
        virtual void DispatchMesh( uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ )                     = 0;
        virtual void DrawIndirect( IBufferResource *buffer, uint64_t offset, uint32_t drawCount, uint32_t stride )        = 0;
        virtual void DrawIndexedIndirect( IBufferResource *buffer, uint64_t offset, uint32_t drawCount, uint32_t stride ) = 0;
        virtual void DispatchIndirect( IBufferResource *buffer, uint64_t offset )                                         = 0;

        virtual const QueueType GetQueueType( ) = 0;
    };

    struct DZ_API ICommandListArray
    {
        ICommandList **Elements;
        uint32_t       NumElements;
    };
} // namespace DenOfIz
