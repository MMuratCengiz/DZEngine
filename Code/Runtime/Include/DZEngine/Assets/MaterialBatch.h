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

#include <DenOfIzGraphics/DenOfIzGraphics.h>
#include "DZEngine/Components/AssetHandle.h"
#include "MeshAssetData.h"

namespace DZEngine
{
    struct MaterialBatchDesc
    {
        ILogicalDevice *LogicalDevice;
    };

    struct MaterialDataRequest
    {
        Float4 BaseColorFactor;
        float  MetallicFactor;
        float  RoughnessFactor;
        float  NormalScale;
        float  OcclusionStrength;
        Float4 EmissiveFactor;

        TextureHandle Albedo;
        TextureHandle Normal;
        TextureHandle Roughness;
        TextureHandle Metallic;
        TextureHandle Emissive;
        TextureHandle Occlusion;
        TextureHandle Custom0;
        TextureHandle Custom1;
    };

    struct MaterialData : MaterialDataRequest
    {
        MaterialHandle Handle;
    };

    struct TextureData
    {
        TextureHandle     Handle;
        ITextureResource *Texture;
    };

    class MaterialBatch
    {
        ILogicalDevice                    *m_logicalDevice;
        std::unique_ptr<BatchResourceCopy> m_batchResourceCopy;

        std::vector<ITextureResource *>            m_textures;
        std::vector<std::unique_ptr<MaterialData>> m_materialData;

        std::unordered_map<std::string, TextureHandle>  m_texAliases;
        std::unordered_map<std::string, MaterialHandle> m_matAliases;

        std::mutex m_nextTexHandleLock;
        size_t     m_nextTexHandle = 0;

        std::mutex m_nextMatHandleLock;
        size_t     m_nextMatHandle = 0;

    public:
        explicit MaterialBatch( const MaterialBatchDesc &desc );
        ~MaterialBatch( ) = default;

        void BeginUpdate( );
        void EndUpdate( ISemaphore *onComplete = nullptr ); // nullptr will block execution

        TextureHandle  LoadTexture( const std::string &alias, BinaryReader &reader );
        TextureHandle  AddTexture( const std::string &alias, ITextureResource *texture );
        MaterialHandle AddMaterial( const std::string &alias, MaterialDataRequest material );

        MaterialData *GetMaterial( const std::string &alias );
        MaterialData *GetMaterial( MaterialHandle handle ) const;

        ITextureResource *GetTexture( const std::string &alias );
        ITextureResource *GetTexture( TextureHandle handle ) const;

        std::vector<TextureData> GetTextures( ) const;

    private:
        size_t NextTextureHandle( const std::string &alias );
        size_t NextMaterialHandle( const std::string &alias );
    };
} // namespace DZEngine
