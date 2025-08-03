find_package(flecs CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)

add_library(DZRuntime STATIC)

target_include_directories(DZRuntime PUBLIC Include)

target_sources(DZRuntime PRIVATE
        Source/Assets/AssetBatcher.cpp
        Source/Assets/MeshAssetData.cpp
        Source/Assets/MeshBatch.cpp
        Source/Input/InputSystem.cpp
        Source/Rendering/RenderLoop.cpp
        Source/Scene/Scene.cpp
        Source/Scene/World.cpp
        Source/Utilities/MathConverter.cpp
        Source/DummyGame.cpp
        Source/GameRunner.cpp
)

target_link_libraries(DZRuntime PUBLIC DenOfIz::DenOfIzGraphics flecs::flecs spdlog::spdlog fmt::fmt)