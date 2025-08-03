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

AssetBatcher::AssetBatcher( const AssetBatcherDesc &desc ) : m_graphicsContext( desc.GraphicsContext )
{
    m_batches.reserve( 1024 );
}

size_t AssetBatcher::AddBatch( const std::string &alias )
{
    std::lock_guard lock( m_addBatchMutex );

    const size_t index = m_batches.size( );

    MeshBatchDesc batchDesc{ };
    batchDesc.LogicalDevice = m_graphicsContext->LogicalDevice;
    const auto batch        = new MeshBatch( batchDesc );

    auto assetBatch       = std::make_unique<AssetBatch>( );
    assetBatch->MeshBatch = std::unique_ptr<MeshBatch>( batch );
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
}

void AssetBatcher::EndBatchUpdate( size_t batchId ) const
{
    if ( batchId >= m_batches.size( ) )
    {
        spdlog::error( "Invalid batch id: {}, call AddBatch first", batchId );
        return;
    }

    m_batches[ batchId ]->MeshBatch->EndUpdate( );
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
