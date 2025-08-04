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

#include "DZEngine/Assets/SkeletonBatch.h"
#include <spdlog/spdlog.h>

using namespace DZEngine;

SkeletonBatch::SkeletonBatch( const SkeletonBatchDesc &desc ) : m_logicalDevice( desc.LogicalDevice )
{
    if ( !m_logicalDevice )
    {
        spdlog::error( "SkeletonBatch: LogicalDevice is required" );
        return;
    }
    m_batchResourceCopy = std::make_unique<BatchResourceCopy>( m_logicalDevice );
}

void SkeletonBatch::BeginUpdate( )
{
    if ( !m_batchResourceCopy )
    {
        m_batchResourceCopy = std::make_unique<BatchResourceCopy>( m_logicalDevice );
    }
    m_batchResourceCopy->Begin( );
}

void SkeletonBatch::EndUpdate( ISemaphore *onComplete )
{
    if ( !m_batchResourceCopy )
    {
        spdlog::error( "BeginUpdate not called" );
        return;
    }
    m_batchResourceCopy->Submit( onComplete );
    if ( !onComplete )
    {
        m_batchResourceCopy.reset( );
    }
}

SkeletonHandle SkeletonBatch::LoadSkeleton( const std::string &alias, BinaryReader &reader )
{
    SkeletonAssetReaderDesc readerDesc{ };
    readerDesc.Reader = &reader;
    SkeletonAssetReader skelReader( readerDesc );

    const SkeletonAsset *skelAsset = skelReader.Read( );
    if ( !skelAsset )
    {
        spdlog::error( "SkeletonBatch::LoadSkeleton - Failed to read skeleton asset" );
        return InvalidSkeletonHandle;
    }

    const SkeletonAssetData skelData = SkeletonAssetData::LoadFromSkeletonAsset( *skelAsset );
    return AddSkeleton( alias, skelData );
}

SkeletonHandle SkeletonBatch::AddSkeleton( const std::string &alias, const SkeletonAssetData &skeletonData )
{
    const size_t nextSkelHandle              = NextSkeletonHandle( alias );
    m_skeletonData[ nextSkelHandle ]         = std::make_unique<SkeletonAssetData>( skeletonData );
    m_skeletonData[ nextSkelHandle ]->Handle = SkeletonHandle( nextSkelHandle );
    return SkeletonHandle( nextSkelHandle );
}

SkeletonAssetData *SkeletonBatch::GetSkeleton( const std::string &alias )
{
    const auto it = m_skelAliases.find( alias );
    if ( it == m_skelAliases.end( ) )
    {
        return nullptr;
    }
    return m_skeletonData[ it->second.Id ].get( );
}

SkeletonAssetData *SkeletonBatch::GetSkeleton( const SkeletonHandle handle ) const
{
    if ( handle.Id >= m_skeletonData.size( ) )
    {
        return nullptr;
    }
    return m_skeletonData[ handle.Id ].get( );
}

size_t SkeletonBatch::NextSkeletonHandle( const std::string &alias )
{
    std::lock_guard lock( m_nextSkelHandleLock );
    m_nextSkelHandle++;
    if ( m_nextSkelHandle >= m_skeletonData.size( ) )
    {
        m_skeletonData.resize( m_nextSkelHandle + 1 );
    }
    m_skelAliases[ alias ] = SkeletonHandle( m_nextSkelHandle );
    return m_nextSkelHandle;
}
