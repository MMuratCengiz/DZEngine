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

#include "DZEngine/Scene/SceneLoader.h"
#include "DZEngine/Scene/ComponentSerialization.h"
#include "DZEngine/Scene/Scene.h"

#include <fstream>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

using namespace DZEngine;
using json = nlohmann::json;

LoadResult SceneLoader::LoadSceneFromFile( Scene *scene, const std::filesystem::path &filePath )
{
    if ( !scene )
    {
        spdlog::error( "SceneLoader::LoadSceneFromFile - Scene is null" );
        return LoadResult::SceneNull;
    }

    if ( !std::filesystem::exists( filePath ) )
    {
        spdlog::error( "SceneLoader::LoadSceneFromFile - File does not exist: {}", filePath.string( ) );
        return LoadResult::FileNotFound;
    }

    std::ifstream file( filePath );
    if ( !file.is_open( ) )
    {
        spdlog::error( "SceneLoader::LoadSceneFromFile - Failed to open file: {}", filePath.string( ) );
        return LoadResult::FileOpenError;
    }

    const std::string jsonData( ( std::istreambuf_iterator<char>( file ) ), std::istreambuf_iterator<char>( ) );
    file.close( );

    if ( file.fail( ) )
    {
        spdlog::error( "SceneLoader::LoadSceneFromFile - Failed to read from file: {}", filePath.string( ) );
        return LoadResult::FileOpenError;
    }

    const LoadResult result = LoadSceneFromJson( scene, jsonData );
    if ( result == LoadResult::Success )
    {
        spdlog::info( "Scene loaded from: {}", filePath.string( ) );
    }

    return result;
}

LoadResult SceneLoader::LoadSceneFromJson( Scene *scene, const std::string &jsonData )
{
    if ( !scene )
    {
        spdlog::error( "SceneLoader::LoadSceneFromJson - Scene is null" );
        return LoadResult::SceneNull;
    }

    if ( !json::accept( jsonData ) )
    {
        spdlog::error( "SceneLoader::LoadSceneFromJson - Invalid JSON data" );
        return LoadResult::JsonParseError;
    }

    json sceneJson = json::parse( jsonData );

    const flecs::entity root  = scene->GetRoot( );
    flecs::world        world = root.world( );

    ComponentSerialization::RegisterAllComponents( world );

    scene->Clear( );

    if ( sceneJson.contains( "entities" ) && sceneJson[ "entities" ].is_array( ) )
    {
        for ( const auto &entityJson : sceneJson[ "entities" ] )
        {
            std::string   entityJsonStr = entityJson.dump( );
            flecs::entity entity        = world.entity( ).child_of( root );
            entity.from_json( entityJsonStr.c_str( ) );
        }
    }

    return LoadResult::Success;
}