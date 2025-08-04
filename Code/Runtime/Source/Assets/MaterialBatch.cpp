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

#include "DZEngine/Assets/MaterialBatch.h"
#include <spdlog/spdlog.h>

using namespace DZEngine;

MaterialBatch::MaterialBatch( const MaterialBatchDesc &desc ) : m_logicalDevice( desc.LogicalDevice )
{
    if ( !m_logicalDevice )
    {
        spdlog::error( "MeshPool: LogicalDevice is required" );
        return;
    }
    m_batchResourceCopy = std::make_unique<BatchResourceCopy>( m_logicalDevice );
}

void MaterialBatch::BeginUpdate( )
{
    if ( !m_batchResourceCopy )
    {
        m_batchResourceCopy = std::make_unique<BatchResourceCopy>( m_logicalDevice );
    }
    m_batchResourceCopy->Begin( );
}

void MaterialBatch::EndUpdate( ISemaphore *onComplete )
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

TextureHandle MaterialBatch::LoadTexture( const std::string &alias, BinaryReader &reader )
{
    TextureAssetReaderDesc readerDesc{ };
    readerDesc.Reader = &reader;
    TextureAssetReader texReader( readerDesc );

    CreateAssetTextureDesc assetTextureDesc{ };
    assetTextureDesc.Reader = &texReader;
    ITextureResource *tex   = m_batchResourceCopy->CreateAndLoadAssetTexture( assetTextureDesc );
    return AddTexture( alias, tex );
}

TextureHandle MaterialBatch::AddTexture( const std::string &alias, ITextureResource *texture )
{
    const size_t nextTexHandle  = NextTextureHandle( alias );
    m_textures[ nextTexHandle ] = texture;
    return TextureHandle( nextTexHandle );
}

MaterialHandle MaterialBatch::AddMaterial( const std::string &alias, MaterialDataRequest material )
{
    const size_t nextMatHandle              = NextMaterialHandle( alias );
    m_materialData[ nextMatHandle ]         = std::make_unique<MaterialData>( material );
    m_materialData[ nextMatHandle ]->Handle = MaterialHandle( nextMatHandle );
    return MaterialHandle( nextMatHandle );
}

MaterialData *MaterialBatch::GetMaterial( const std::string &alias )
{
    const auto it = m_matAliases.find( alias );
    if ( it == m_matAliases.end( ) )
    {
        return nullptr;
    }
    return m_materialData[ it->second.Id ].get( );
}

MaterialData *MaterialBatch::GetMaterial( const MaterialHandle handle ) const
{
    if ( handle.Id >= m_materialData.size( ) )
    {
        return nullptr;
    }
    return m_materialData[ handle.Id ].get( );
}

ITextureResource *MaterialBatch::GetTexture( const std::string &alias )
{
    const auto it = m_texAliases.find( alias );
    if ( it == m_texAliases.end( ) )
    {
        return nullptr;
    }
    return m_textures[ it->second.Id ];
}

ITextureResource *MaterialBatch::GetTexture( const TextureHandle handle ) const
{
    if ( handle.Id >= m_textures.size( ) )
    {
        return nullptr;
    }
    return m_textures[ handle.Id ];
}

size_t MaterialBatch::NextTextureHandle( const std::string &alias )
{
    std::lock_guard lock( m_nextTexHandleLock );
    m_nextTexHandle++;
    if ( m_nextTexHandle >= m_textures.size( ) )
    {
        m_textures.resize( m_nextTexHandle + 1 );
    }
    m_texAliases[ alias ] = TextureHandle( m_nextTexHandle );
    return m_nextTexHandle;
}

size_t MaterialBatch::NextMaterialHandle( const std::string &alias )
{
    std::lock_guard lock( m_nextMatHandleLock );
    m_nextMatHandle++;
    if ( m_nextMatHandle >= m_materialData.size( ) )
    {
        m_materialData.resize( m_nextMatHandle + 1 );
    }
    m_matAliases[ alias ] = MaterialHandle( m_nextMatHandle );
    return m_nextMatHandle;
}
