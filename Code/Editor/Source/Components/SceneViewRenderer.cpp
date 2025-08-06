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

#include "DZEngine/Components/SceneViewRenderer.h"
#include <spdlog/spdlog.h>
#include "DZEngine/Assets/MaterialBatch.h"
#include "DZEngine/Components/CameraComponent.h"
#include "DZEngine/Components/Graphics/MaterialComponent.h"
#include "DZEngine/Components/Graphics/MeshComponent.h"
#include "DZEngine/Components/Graphics/RenderableComponent.h"
#include "DZEngine/Components/TransformComponent.h"
#include "DZEngine/Systems/SceneViewCameraSystem.h"
#include "DenOfIzGraphics/Data/BatchResourceCopy.h"

using namespace DZEngine;
using namespace DenOfIz;

SceneViewRenderer::SceneViewRenderer( const SceneViewRendererDesc &desc )
{
    m_logicalDevice     = desc.AppContext->GraphicsContext->LogicalDevice;
    m_viewport          = desc.Viewport;
    m_numFramesInFlight = desc.NumFramesInFlight;
    m_assets            = desc.AppContext->AssetBatcher;
    m_ecsWorld          = &desc.AppContext->World->GetWorld( );
    m_resourceTracking  = desc.AppContext->GraphicsContext->ResourceTracking;

    desc.AppContext->World->RegisterSystem<SceneViewCameraSystem>( );

    CreateRenderTargets( );
    CreateAssets( );
    CreateScene( );

    RendererDesc rendererDesc{ };
    rendererDesc.AppContext = desc.AppContext;

    m_renderer = std::make_unique<GPUDrivenRenderer>( rendererDesc );
}

void SceneViewRenderer::UpdateViewport( const Viewport &viewport )
{
    if ( m_viewport.Width != viewport.Width || m_viewport.Height != viewport.Height )
    {
        m_viewport = viewport;
        CreateRenderTargets( );
    }
}

ISemaphore *SceneViewRenderer::Render( const uint32_t frameIndex ) const
{
    RenderFrameDesc renderFrameDesc{ };
    renderFrameDesc.FrameIndex   = frameIndex;
    renderFrameDesc.Viewport     = m_viewport;
    renderFrameDesc.RenderTarget = GetRenderTarget( frameIndex );
    return m_renderer->RenderFrame( renderFrameDesc );
}

ITextureResource *SceneViewRenderer::GetRenderTarget( const uint32_t frameIndex ) const
{
    if ( frameIndex < m_renderTargets.size( ) && m_renderTargets[ frameIndex ] )
    {
        return m_renderTargets[ frameIndex ].get( );
    }
    spdlog::error( "Invalid frame index" );
    return nullptr;
}

void SceneViewRenderer::CreateAssets( ) const
{
    BoxDesc boxDesc{ };
    boxDesc.BuildDesc = 0;
    boxDesc.Width     = 1.0f;
    boxDesc.Height    = 1.0f;
    boxDesc.Depth     = 1.0f;

    const auto box = std::unique_ptr<GeometryData>( Geometry::BuildBox( boxDesc ) );
    m_assets->BeginBatchUpdate( );
    m_assets->AddGeometry( box.get( ), "Box1" );

    SphereDesc sphereDesc{ };
    sphereDesc.BuildDesc    = 0;
    sphereDesc.Diameter     = 1.0f;
    sphereDesc.Tessellation = 24.0f;
    const auto sphere       = std::unique_ptr<GeometryData>( Geometry::BuildSphere( sphereDesc ) );
    m_assets->AddGeometry( sphere.get( ), "Sphere1" );

    MaterialDataRequest redMaterial{ };
    redMaterial.BaseColorFactor   = { 0.8f, 0.2f, 0.2f, 1.0f };
    redMaterial.MetallicFactor    = 0.1f;
    redMaterial.RoughnessFactor   = 0.7f;
    redMaterial.NormalScale       = 1.0f;
    redMaterial.OcclusionStrength = 1.0f;
    redMaterial.EmissiveFactor    = { 0.0f, 0.0f, 0.0f, 0.0f };
    m_assets->AddMaterial( "RedMaterial", redMaterial );

    MaterialDataRequest blueMaterial{ };
    blueMaterial.BaseColorFactor   = { 0.2f, 0.4f, 0.9f, 1.0f };
    blueMaterial.MetallicFactor    = 0.3f;
    blueMaterial.RoughnessFactor   = 0.4f;
    blueMaterial.NormalScale       = 1.0f;
    blueMaterial.OcclusionStrength = 1.0f;
    blueMaterial.EmissiveFactor    = { 0.0f, 0.0f, 0.0f, 0.0f };
    m_assets->AddMaterial( "BlueMaterial", blueMaterial );

    m_assets->EndBatchUpdate( );
}

void SceneViewRenderer::CreateScene( ) const
{
    const auto boxMesh    = m_assets->Mesh( 0 )->GetSubMesh( "Box1" );
    const auto sphereMesh = m_assets->Mesh( 0 )->GetSubMesh( "Sphere1" );

    const auto redBox = m_ecsWorld->entity( "RedBox" );
    redBox.add<TransformComponent>( );
    redBox.add<MeshComponent>( );
    redBox.add<MaterialComponent>( );
    redBox.add<RenderableComponent>( );

    auto &redBoxTransform    = redBox.get_mut<TransformComponent>( );
    redBoxTransform.Position = { -2.0f, 0.0f, 0.0f };
    redBoxTransform.Rotation = { 0.0f, 0.0f, 0.0f, 1.0f };
    redBoxTransform.Scale    = { 1.0f, 1.0f, 1.0f };

    auto &redBoxMesh   = redBox.get_mut<MeshComponent>( );
    redBoxMesh.BatchId = 0;
    redBoxMesh.Handle  = boxMesh.Handle;

    auto &redBoxMaterial  = redBox.get_mut<MaterialComponent>( );
    redBoxMaterial.Handle = m_assets->Material( ) ->GetMaterial( "RedMaterial" )->Handle;

    auto &redBoxRenderable   = redBox.get_mut<RenderableComponent>( );
    redBoxRenderable.Visible = true;

    const auto blueSphere = m_ecsWorld->entity( "BlueSphere" );
    blueSphere.add<TransformComponent>( );
    blueSphere.add<MeshComponent>( );
    blueSphere.add<MaterialComponent>( );
    blueSphere.add<RenderableComponent>( );

    auto &blueSphereTransform    = blueSphere.get_mut<TransformComponent>( );
    blueSphereTransform.Position = { 2.0f, 0.0f, 0.0f };
    blueSphereTransform.Rotation = { 0.0f, 0.0f, 0.0f, 1.0f };
    blueSphereTransform.Scale    = { 1.0f, 1.0f, 1.0f };

    auto &blueSphereMesh   = blueSphere.get_mut<MeshComponent>( );
    blueSphereMesh.BatchId = 0;
    blueSphereMesh.Handle  = sphereMesh.Handle;

    auto &blueSphereMaterial  = blueSphere.get_mut<MaterialComponent>( );
    blueSphereMaterial.Handle = m_assets->Material( ) ->GetMaterial( "BlueMaterial" )->Handle;

    auto &blueSphereRenderable   = blueSphere.get_mut<RenderableComponent>( );
    blueSphereRenderable.Visible = true;

    const auto greenBox = m_ecsWorld->entity( "GreenBox" );
    greenBox.add<TransformComponent>( );
    greenBox.add<MeshComponent>( );
    greenBox.add<RenderableComponent>( );

    auto &greenBoxTransform    = greenBox.get_mut<TransformComponent>( );
    greenBoxTransform.Position = { 0.0f, 2.0f, 0.0f };
    greenBoxTransform.Rotation = { 0.0f, 0.0f, 0.0f, 1.0f };
    greenBoxTransform.Scale    = { 0.5f, 0.5f, 0.5f };

    auto &greenBoxMesh   = greenBox.get_mut<MeshComponent>( );
    greenBoxMesh.BatchId = 0;
    greenBoxMesh.Handle  = boxMesh.Handle;

    auto &greenBoxRenderable   = greenBox.get_mut<RenderableComponent>( );
    greenBoxRenderable.Visible = true;
}

void SceneViewRenderer::CreateRenderTargets( )
{
    if ( m_viewport.Width <= 0 || m_viewport.Height <= 0 )
    {
        return;
    }

    m_renderTargets.clear( );
    m_depthTextures.clear( );

    for ( uint32_t i = 0; i < m_numFramesInFlight; ++i )
    {
        TextureDesc renderTargetDesc{ };
        renderTargetDesc.Width      = static_cast<uint32_t>( m_viewport.Width );
        renderTargetDesc.Height     = static_cast<uint32_t>( m_viewport.Height );
        renderTargetDesc.Format     = Format::B8G8R8A8Unorm;
        renderTargetDesc.Descriptor = ResourceDescriptor::RenderTarget | ResourceDescriptor::Texture;
        renderTargetDesc.DebugName  = "SceneViewRenderTarget";

        m_renderTargets.push_back( std::unique_ptr<ITextureResource>( m_logicalDevice->CreateTextureResource( renderTargetDesc ) ) );

        m_resourceTracking->TrackTexture( m_renderTargets[ i ].get( ), ResourceUsage::ShaderResource );

        TextureDesc depthDesc{ };
        depthDesc.Width      = static_cast<uint32_t>( m_viewport.Width );
        depthDesc.Height     = static_cast<uint32_t>( m_viewport.Height );
        depthDesc.Format     = Format::D32Float;
        depthDesc.Descriptor = ResourceDescriptor::DepthStencil;
        depthDesc.Usages     = ResourceUsage::DepthWrite;
        depthDesc.DebugName  = "SceneViewDepthBuffer";

        m_depthTextures.push_back( std::unique_ptr<ITextureResource>( m_logicalDevice->CreateTextureResource( depthDesc ) ) );
        m_resourceTracking->TrackTexture( m_depthTextures[ i ].get( ), ResourceUsage::DepthWrite );
    }
}
