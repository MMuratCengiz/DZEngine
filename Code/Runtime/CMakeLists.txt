find_package(flecs CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)

add_library(DZRuntime STATIC)

target_include_directories(DZRuntime PUBLIC Include)

target_sources(DZRuntime PRIVATE
        Source/Assets/MeshAssetData.cpp
        Source/Assets/MeshPool.cpp
        Source/Assets/SceneAssets.cpp
        Source/Rendering/RenderLoop.cpp
        Source/Scene/Scene.cpp
        Source/Scene/World.cpp
        Source/DummyGame.cpp
        Source/GameRunner.cpp
)

target_link_libraries(DZRuntime PUBLIC DenOfIz::DenOfIzGraphics flecs::flecs spdlog::spdlog fmt::fmt)