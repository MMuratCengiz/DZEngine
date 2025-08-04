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

#include "DZEngine/Assets/AssetBatcher.h"
#include <spdlog/spdlog.h>

using namespace DZEngine;

AssetBatcher::AssetBatcher( const AssetBatcherDesc &desc ) : m_graphicsContext( desc.GraphicsContext ), m_assetBundle( desc.AssetBundle ), m_assetRegistry( desc.AssetRegistry )
{
    m_batches.reserve( 1024 );
}

size_t AssetBatcher::AddBatch( const std::string &alias )
{
    std::lock_guard lock( m_addBatchMutex );

    const size_t index = m_batches.size( );

    MeshBatchDesc batchDesc{ };
    batchDesc.LogicalDevice = m_graphicsContext->LogicalDevice;
    const auto meshBatch    = new MeshBatch( batchDesc );

    MaterialBatchDesc matBatchDesc{ };
    matBatchDesc.LogicalDevice = m_graphicsContext->LogicalDevice;
    const auto matBatch        = new MaterialBatch( matBatchDesc );

    AnimationBatchDesc animBatchDesc{ };
    animBatchDesc.LogicalDevice = m_graphicsContext->LogicalDevice;
    const auto animBatch        = new AnimationBatch( animBatchDesc );

    SkeletonBatchDesc skelBatchDesc{ };
    skelBatchDesc.LogicalDevice = m_graphicsContext->LogicalDevice;
    const auto skelBatch        = new SkeletonBatch( skelBatchDesc );

    auto assetBatch            = std::make_unique<AssetBatch>( );
    assetBatch->MeshBatch      = std::unique_ptr<MeshBatch>( meshBatch );
    assetBatch->MaterialBatch  = std::unique_ptr<MaterialBatch>( matBatch );
    assetBatch->AnimationBatch = std::unique_ptr<AnimationBatch>( animBatch );
    assetBatch->SkeletonBatch  = std::unique_ptr<SkeletonBatch>( skelBatch );
    m_batches.emplace_back( std::move( assetBatch ) );
    m_batchAliases[ alias ] = index;

    return index;
}

void AssetBatcher::BeginBatchUpdate( size_t batchId ) const
{
    if ( batchId >= m_batches.size( ) )
    {
        spdlog::error( "Invalid batch id: {}, call AddBatch first", batchId );
        return;
    }

    m_batches[ batchId ]->MeshBatch->BeginUpdate( );
    m_batches[ batchId ]->MaterialBatch->BeginUpdate( );
    m_batches[ batchId ]->AnimationBatch->BeginUpdate( );
    m_batches[ batchId ]->SkeletonBatch->BeginUpdate( );
}

void AssetBatcher::EndBatchUpdate( size_t batchId ) const
{
    if ( batchId >= m_batches.size( ) )
    {
        spdlog::error( "Invalid batch id: {}, call AddBatch first", batchId );
        return;
    }

    m_batches[ batchId ]->MeshBatch->EndUpdate( );
    m_batches[ batchId ]->MaterialBatch->EndUpdate( );
    m_batches[ batchId ]->AnimationBatch->EndUpdate( );
    m_batches[ batchId ]->SkeletonBatch->EndUpdate( );
}

void AssetBatcher::AddMesh( BinaryReader &reader, const std::vector<std::string> &submeshAliases ) const
{
    return AddMesh( 0, reader, submeshAliases );
}

void AssetBatcher::AddGeometry( const GeometryData *data, const std::string &alias ) const
{
    return AddGeometry( 0, data, alias );
}

void AssetBatcher::AddMesh( size_t batchId, BinaryReader &reader, const std::vector<std::string> &submeshAliases ) const
{
    if ( batchId >= m_batches.size( ) )
    {
        spdlog::error( "Invalid batch id: {}, call AddBatch first", batchId );
        return;
    }

    m_batches[ batchId ]->MeshBatch->AddMesh( reader, submeshAliases );
}

void AssetBatcher::AddGeometry( size_t batchId, const GeometryData *data, const std::string &alias ) const
{
    if ( batchId >= m_batches.size( ) )
    {
        spdlog::error( "Invalid batch id: {}, call AddBatch first", batchId );
        return;
    }

    m_batches[ batchId ]->MeshBatch->AddGeometry( data, alias );
}

MeshBatch const *AssetBatcher::Mesh( size_t batchId ) const
{
    if ( batchId >= m_batches.size( ) )
    {
        spdlog::error( "Invalid batch id: {}, call AddBatch first", batchId );
        return nullptr;
    }
    return m_batches[ batchId ]->MeshBatch.get( );
}

MaterialBatch const *AssetBatcher::Material( size_t batchId ) const
{
    if ( batchId >= m_batches.size( ) )
    {
        spdlog::error( "Invalid batch id: {}, call AddBatch first", batchId );
        return nullptr;
    }
    return m_batches[ batchId ]->MaterialBatch.get( );
}

MeshHandle AssetBatcher::AddMesh( size_t batchId, const std::string &uri, const std::vector<std::string> &submeshAliases ) const
{
    if ( batchId >= m_batches.size( ) )
    {
        spdlog::error( "AssetBatcher::AddMeshFromUri - Invalid batch id: {}", batchId );
        return InvalidMeshHandle;
    }

    const auto reader = m_assetBundle->LoadAsset( uri );
    if ( !reader )
    {
        spdlog::error( "AssetBatcher::AddMeshFromUri - Failed to load asset: {}", uri );
        return InvalidMeshHandle;
    }

    if ( const GPUMesh gpuMesh = m_batches[ batchId ]->MeshBatch->AddMesh( *reader, submeshAliases ); !gpuMesh.SubMeshes.empty( ) )
    {
        MeshHandle meshHandle = gpuMesh.SubMeshes[ 0 ].Handle;
        m_assetRegistry->RegisterMeshAsset( batchId, meshHandle, uri );

        spdlog::info( "AssetBatcher::AddMeshFromUri - Added mesh from {} to batch {} with handle {}", uri, batchId, meshHandle.Id );
        return meshHandle;
    }

    spdlog::error( "AssetBatcher::AddMeshFromUri - Failed to get valid handle for mesh: {}", uri );
    return InvalidMeshHandle;
}

TextureHandle AssetBatcher::LoadTexture( const std::string &alias, const std::string &uri ) const
{
    return LoadTexture( 0, alias, uri );
}

TextureHandle AssetBatcher::LoadTexture( size_t batchId, const std::string &alias, const std::string &uri ) const
{
    if ( batchId >= m_batches.size( ) )
    {
        spdlog::error( "AssetBatcher::LoadTexture - Invalid batch id: {}", batchId );
        return InvalidTextureHandle;
    }
    const auto reader = m_assetBundle->LoadAsset( uri );
    if ( !reader )
    {
        spdlog::error( "AssetBatcher::AddMeshFromUri - Failed to load asset: {}", uri );
        return InvalidTextureHandle;
    }

    return m_batches[ batchId ]->MaterialBatch->LoadTexture( alias, *reader );
}

TextureHandle AssetBatcher::AddTexture( const std::string &alias, ITextureResource *texture ) const
{
    return AddTexture( 0, alias, texture );
}

TextureHandle AssetBatcher::AddTexture( size_t batchId, const std::string &alias, ITextureResource *texture ) const
{
    if ( batchId >= m_batches.size( ) )
    {
        spdlog::error( "AssetBatcher::AddTexture - Invalid batch id: {}", batchId );
        return InvalidTextureHandle;
    }
    return m_batches[ batchId ]->MaterialBatch->AddTexture( alias, texture );
}

MaterialHandle AssetBatcher::AddMaterial( const std::string &alias, const MaterialDataRequest &material ) const
{
    return AddMaterial( 0, alias, material );
}

MaterialHandle AssetBatcher::AddMaterial( size_t batchId, const std::string &alias, const MaterialDataRequest &material ) const
{
    if ( batchId >= m_batches.size( ) )
    {
        spdlog::error( "AssetBatcher::AddMaterial - Invalid batch id: {}", batchId );
        return InvalidMaterialHandle;
    }
    return m_batches[ batchId ]->MaterialBatch->AddMaterial( alias, material );
}

AnimationClipHandle AssetBatcher::AddAnimation( size_t batchId, const std::string &uri, const std::string &alias ) const
{
    if ( batchId >= m_batches.size( ) )
    {
        spdlog::error( "AssetBatcher::AddAnimationFromUri - Invalid batch id: {}", batchId );
        return InvalidAnimationClipHandle;
    }

    const auto reader = m_assetBundle->LoadAsset( uri );
    if ( !reader )
    {
        spdlog::error( "AssetBatcher::AddAnimationFromUri - Failed to load asset: {}", uri );
        return InvalidAnimationClipHandle;
    }

    const AnimationClipHandle handle = m_batches[ batchId ]->AnimationBatch->LoadAnimation( alias, *reader );
    if ( handle.IsValid( ) )
    {
        m_assetRegistry->RegisterAnimationAsset( batchId, handle, uri );
        spdlog::info( "AssetBatcher::AddAnimationFromUri - Added animation from {} to batch {} with handle {}", uri, batchId, handle.Id );
    }
    else
    {
        spdlog::error( "AssetBatcher::AddAnimationFromUri - Failed to get valid handle for animation: {}", uri );
    }

    return handle;
}

AnimationClipHandle AssetBatcher::AddAnimation( const std::string &uri, const std::string &alias ) const
{
    return AddAnimation( 0, uri, alias );
}

SkeletonHandle AssetBatcher::AddSkeleton( size_t batchId, const std::string &uri, const std::string &alias ) const
{
    if ( batchId >= m_batches.size( ) )
    {
        spdlog::error( "AssetBatcher::AddSkeletonFromUri - Invalid batch id: {}", batchId );
        return InvalidSkeletonHandle;
    }

    const auto reader = m_assetBundle->LoadAsset( uri );
    if ( !reader )
    {
        spdlog::error( "AssetBatcher::AddSkeletonFromUri - Failed to load asset: {}", uri );
        return InvalidSkeletonHandle;
    }

    const SkeletonHandle handle = m_batches[ batchId ]->SkeletonBatch->LoadSkeleton( alias, *reader );
    if ( handle.IsValid( ) )
    {
        m_assetRegistry->RegisterSkeletonAsset( batchId, handle, uri );
        spdlog::info( "AssetBatcher::AddSkeletonFromUri - Added skeleton from {} to batch {} with handle {}", uri, batchId, handle.Id );
    }
    else
    {
        spdlog::error( "AssetBatcher::AddSkeletonFromUri - Failed to get valid handle for skeleton: {}", uri );
    }

    return handle;
}

SkeletonHandle AssetBatcher::AddSkeleton( const std::string &uri, const std::string &alias ) const
{
    return AddSkeleton( 0, uri, alias );
}

AnimationBatch const *AssetBatcher::Animation( size_t batchId ) const
{
    if ( batchId >= m_batches.size( ) )
    {
        spdlog::error( "Invalid batch id: {}, call AddBatch first", batchId );
        return nullptr;
    }
    return m_batches[ batchId ]->AnimationBatch.get( );
}

SkeletonBatch const *AssetBatcher::Skeleton( size_t batchId ) const
{
    if ( batchId >= m_batches.size( ) )
    {
        spdlog::error( "Invalid batch id: {}, call AddBatch first", batchId );
        return nullptr;
    }
    return m_batches[ batchId ]->SkeletonBatch.get( );
}
