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

#include "DZEngine/Assets/AnimationBatch.h"
#include <spdlog/spdlog.h>

using namespace DZEngine;

AnimationBatch::AnimationBatch( const AnimationBatchDesc &desc ) : m_logicalDevice( desc.LogicalDevice )
{
    if ( !m_logicalDevice )
    {
        spdlog::error( "AnimationBatch: LogicalDevice is required" );
        return;
    }
    m_batchResourceCopy = std::make_unique<BatchResourceCopy>( m_logicalDevice );
}

void AnimationBatch::BeginUpdate( )
{
    if ( !m_batchResourceCopy )
    {
        m_batchResourceCopy = std::make_unique<BatchResourceCopy>( m_logicalDevice );
    }
    m_batchResourceCopy->Begin( );
}

void AnimationBatch::EndUpdate( ISemaphore *onComplete )
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

AnimationClipHandle AnimationBatch::LoadAnimation( const std::string &alias, BinaryReader &reader )
{
    AnimationAssetReaderDesc readerDesc{ };
    readerDesc.Reader = &reader;
    AnimationAssetReader animReader( readerDesc );

    const AnimationAsset *animAsset = animReader.Read( );
    if ( !animAsset )
    {
        spdlog::error( "AnimationBatch::LoadAnimation - Failed to read animation asset" );
        return InvalidAnimationClipHandle;
    }

    const AnimationAssetData animData = AnimationAssetData::LoadFromAnimationAsset( *animAsset );
    return AddAnimation( alias, animData );
}

AnimationClipHandle AnimationBatch::AddAnimation( const std::string &alias, const AnimationAssetData &animationData )
{
    const size_t nextAnimHandle               = NextAnimationHandle( alias );
    m_animationData[ nextAnimHandle ]         = std::make_unique<AnimationAssetData>( animationData );
    m_animationData[ nextAnimHandle ]->Handle = AnimationClipHandle( nextAnimHandle );
    return AnimationClipHandle( nextAnimHandle );
}

AnimationAssetData *AnimationBatch::GetAnimation( const std::string &alias )
{
    const auto it = m_animAliases.find( alias );
    if ( it == m_animAliases.end( ) )
    {
        return nullptr;
    }
    return m_animationData[ it->second.Id ].get( );
}

AnimationAssetData *AnimationBatch::GetAnimation( const AnimationClipHandle handle ) const
{
    if ( handle.Id >= m_animationData.size( ) )
    {
        return nullptr;
    }
    return m_animationData[ handle.Id ].get( );
}

size_t AnimationBatch::NextAnimationHandle( const std::string &alias )
{
    std::lock_guard lock( m_nextAnimHandleLock );
    m_nextAnimHandle++;
    if ( m_nextAnimHandle >= m_animationData.size( ) )
    {
        m_animationData.resize( m_nextAnimHandle + 1 );
    }
    m_animAliases[ alias ] = AnimationClipHandle( m_nextAnimHandle );
    return m_nextAnimHandle;
}
