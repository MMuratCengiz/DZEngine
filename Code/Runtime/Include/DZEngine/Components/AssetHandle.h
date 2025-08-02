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

#include <cstdint>
#include <functional>

namespace DZEngine
{
    /**
     * Handle format:
     * - 32-bit ID with UINT32_MAX representing invalid handle
     * - Type-safe via inheritance
     * - Lightweight for ECS components
     * - Thread-safe comparison and validation
     */
    template <typename T>
    struct AssetHandle
    {
        static constexpr uint32_t Invalid = UINT32_MAX;
        uint32_t                  Id      = Invalid;

        constexpr AssetHandle( ) = default;
        constexpr explicit AssetHandle( const uint32_t id ) : Id( id )
        {
        }
        constexpr bool IsValid( ) const
        {
            return Id != Invalid;
        }
        constexpr void Reset( )
        {
            Id = Invalid;
        }
        constexpr bool operator==( const AssetHandle &Other ) const
        {
            return Id == Other.Id;
        }
        constexpr bool operator!=( const AssetHandle &Other ) const
        {
            return Id != Other.Id;
        }
        constexpr bool operator<( const AssetHandle &Other ) const
        {
            return Id < Other.Id;
        }
    };

    struct TAnimationClipHandle;
    struct TMeshHandle;
    struct TMaterialHandle;
    struct TTextureHandle;
    struct TShaderHandle;
    struct TSceneHandle;
    struct TSkeletonHandle;

    using AnimationClipHandle = AssetHandle<TAnimationClipHandle>;
    using MeshHandle          = AssetHandle<TMeshHandle>;
    using MaterialHandle      = AssetHandle<TMaterialHandle>;
    using TextureHandle       = AssetHandle<TTextureHandle>;
    using ShaderHandle        = AssetHandle<TShaderHandle>;
    using SceneHandle         = AssetHandle<TSceneHandle>;
    using SkeletonHandle      = AssetHandle<TSkeletonHandle>;

    constexpr auto InvalidMeshHandle     = MeshHandle{ };
    constexpr auto InvalidMaterialHandle = MaterialHandle{ };
    constexpr auto InvalidTextureHandle  = TextureHandle{ };
    constexpr auto InvalidShaderHandle   = ShaderHandle{ };
    constexpr auto InvalidSceneHandle    = SceneHandle{ };
} // namespace DZEngine

template <typename T>
struct std::hash<DZEngine::AssetHandle<T>>
{
    std::size_t operator( )( const DZEngine::AssetHandle<T> &h ) const noexcept
    {
        return std::hash<uint32_t>{ }( h.Id );
    }
}; // namespace std
