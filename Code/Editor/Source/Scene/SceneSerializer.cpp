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

#include "DZEngine/Scene/SceneSerializer.h"
#include "DZEngine/Scene/Scene.h"

#include "DZEngine/Components/CameraComponent.h"
#include "DZEngine/Components/Graphics/MaterialComponent.h"
#include "DZEngine/Components/Graphics/MeshComponent.h"
#include "DZEngine/Components/Graphics/RenderableComponent.h"
#include "DZEngine/Components/TransformComponent.h"

#include <DenOfIzGraphics/Utilities/InteropMath.h>
#include <fstream>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

using namespace DZEngine;
using json = nlohmann::json;

SerializationResult SceneSerializer::SaveSceneToFile( const Scene *scene, const std::filesystem::path &filePath )
{
    if ( !scene )
    {
        spdlog::error( "SceneSerializer::SaveSceneToFile - Scene is null" );
        return SerializationResult::SceneNull;
    }

    std::string jsonData;
    if ( const SerializationResult result = SerializeScene( scene, jsonData ); result != SerializationResult::Success )
    {
        return result;
    }

    std::error_code ec;
    std::filesystem::create_directories( filePath.parent_path( ), ec );
    if ( ec )
    {
        spdlog::error( "SceneSerializer::SaveSceneToFile - Failed to create directories: {}", ec.message( ) );
        return SerializationResult::FileOpenError;
    }

    std::ofstream file( filePath );
    if ( !file.is_open( ) )
    {
        spdlog::error( "SceneSerializer::SaveSceneToFile - Failed to create file: {}", filePath.string( ) );
        return SerializationResult::FileOpenError;
    }

    file << jsonData;
    file.close( );

    if ( file.fail( ) )
    {
        spdlog::error( "SceneSerializer::SaveSceneToFile - Failed to write to file: {}", filePath.string( ) );
        return SerializationResult::FileOpenError;
    }

    spdlog::info( "Scene saved to: {}", filePath.string( ) );
    return SerializationResult::Success;
}

SerializationResult SceneSerializer::SerializeScene( const Scene *scene, std::string &outJsonData )
{
    if ( !scene )
    {
        spdlog::error( "SceneSerializer::SerializeScene - Scene is null" );
        outJsonData = "{}";
        return SerializationResult::SceneNull;
    }

    const flecs::entity root  = scene->GetRoot( );
    flecs::world        world = root.world( );

    if ( const SerializationResult regResult = RegisterComponentsForSerialization( world ); regResult != SerializationResult::Success )
    {
        outJsonData = "{}";
        return regResult;
    }

    const auto query = world.query_builder( ).with( flecs::ChildOf, root ).build( );

    json sceneJson;
    sceneJson[ "name" ]     = scene->GetName( );
    sceneJson[ "entities" ] = json::array( );

    query.each(
        [ & ]( const flecs::entity entity )
        {
            std::string entityJsonStr = entity.to_json( ).c_str( );
            if ( entityJsonStr.empty( ) || entityJsonStr == "{}" )
            {
                return;
            }
            if ( !json::accept( entityJsonStr ) )
            {
                spdlog::warn( "SceneSerializer::SerializeScene - Invalid JSON for entity, skipping" );
                return;
            }
            const json entityJson = json::parse( entityJsonStr );
            sceneJson[ "entities" ].push_back( entityJson );
        } );

    outJsonData = sceneJson.dump( 2 );
    return SerializationResult::Success;
}

bool SceneSerializer::ValidateSceneFile( const std::filesystem::path &filePath )
{
    if ( !std::filesystem::exists( filePath ) )
    {
        return false;
    }
    if ( filePath.extension( ) != GetSceneFileExtension( ) )
    {
        return false;
    }
    std::ifstream file( filePath );
    if ( !file.is_open( ) )
    {
        return false;
    }
    std::string jsonData( ( std::istreambuf_iterator<char>( file ) ), std::istreambuf_iterator<char>( ) );
    file.close( );
    if ( !json::accept( jsonData ) )
    {
        return false;
    }
    if ( const json sceneJson = json::parse( jsonData ); !sceneJson.contains( "name" ) || !sceneJson.contains( "entities" ) )
    {
        return false;
    }
    return true;
}

std::string SceneSerializer::GetSceneFileExtension( )
{
    return ".dzscene";
}

SerializationResult SceneSerializer::RegisterComponentsForSerialization( flecs::world &world )
{
    static bool registered = false;
    if ( registered )
    {
        return SerializationResult::Success;
    }

    world.component<Float3>( ).member( "X", &Float3::X ).member( "Y", &Float3::Y ).member( "Z", &Float3::Z );

    world.component<Float4>( ).member( "X", &Float4::X ).member( "Y", &Float4::Y ).member( "Z", &Float4::Z ).member( "W", &Float4::W );

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

    world.component<CameraComponent>( );

    registered = true;
    spdlog::debug( "Components registered for serialization" );
    return SerializationResult::Success;
}
