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

#include "DZEngine/Scene/ComponentSerialization.h"

#include "DZEngine/Components/CameraComponent.h"
#include "DZEngine/Components/Graphics/MaterialComponent.h"
#include "DZEngine/Components/Graphics/MeshComponent.h"
#include "DZEngine/Components/Graphics/RenderableComponent.h"
#include "DZEngine/Components/TransformComponent.h"

#include <DenOfIzGraphics/Utilities/InteropMath.h>
#include <spdlog/spdlog.h>

using namespace DZEngine;

void ComponentSerialization::RegisterAllComponents( flecs::world &world )
{
    static bool registered = false;
    if ( registered )
    {
        return;
    }

    RegisterMathTypes( world );
    RegisterEngineComponents( world );

    registered = true;
    spdlog::debug( "All components registered for serialization/deserialization" );
}

void ComponentSerialization::RegisterMathTypes( flecs::world &world )
{
    world.component<Float3>( ).member( "X", &Float3::X ).member( "Y", &Float3::Y ).member( "Z", &Float3::Z );

    world.component<Float4>( ).member( "X", &Float4::X ).member( "Y", &Float4::Y ).member( "Z", &Float4::Z ).member( "W", &Float4::W );

    world.component<Float4x4>( )
        .member( "_11", &Float4x4::_11 )
        .member( "_12", &Float4x4::_12 )
        .member( "_13", &Float4x4::_13 )
        .member( "_14", &Float4x4::_14 )
        .member( "_21", &Float4x4::_21 )
        .member( "_22", &Float4x4::_22 )
        .member( "_23", &Float4x4::_23 )
        .member( "_24", &Float4x4::_24 )
        .member( "_31", &Float4x4::_31 )
        .member( "_32", &Float4x4::_32 )
        .member( "_33", &Float4x4::_33 )
        .member( "_34", &Float4x4::_34 )
        .member( "_41", &Float4x4::_41 )
        .member( "_42", &Float4x4::_42 )
        .member( "_43", &Float4x4::_43 )
        .member( "_44", &Float4x4::_44 );
}

void ComponentSerialization::RegisterEngineComponents( flecs::world &world )
{
    world.component<TransformComponent>( )
        .member( "Position", &TransformComponent::Position )
        .member( "Rotation", &TransformComponent::Rotation )
        .member( "Scale", &TransformComponent::Scale );

    world.component<RenderableComponent>( )
        .member( "Visible", &RenderableComponent::Visible )
        .member( "CastShadows", &RenderableComponent::CastShadows )
        .member( "ReceiveShadows", &RenderableComponent::ReceiveShadows )
        .member( "RenderLayer", &RenderableComponent::RenderLayer )
        .member( "RenderOrder", &RenderableComponent::RenderOrder );

    world.component<MaterialComponent>( ).member( "Handle", &MaterialComponent::Handle );

    world.component<MeshComponent>( ).member( "BatchId", &MeshComponent::BatchId ).member( "Handle", &MeshComponent::Handle );

    world.component<MeshHandle>( ).member( "Id", &MeshHandle::Id );

    world.component<MaterialHandle>( ).member( "Id", &MaterialHandle::Id );

    world.component<CameraComponent>( )
        .member( "View", &CameraComponent::View )
        .member( "Projection", &CameraComponent::Projection )
        .member( "ViewProjection", &CameraComponent::ViewProjection )
        .member( "Position", &CameraComponent::Position );
}
