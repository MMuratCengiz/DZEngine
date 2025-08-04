find_package(flecs CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)

add_library(DZRuntime STATIC)

target_include_directories(DZRuntime PUBLIC Include)

target_sources(DZRuntime PRIVATE
        Source/Assets/AssetBatcher.cpp
        Source/Assets/AssetBundle.cpp
        Source/Assets/AssetRegistry.cpp
        Source/Assets/MeshAssetData.cpp
        Source/Assets/MeshBatch.cpp
        Source/Assets/AnimationAssetData.cpp
        Source/Assets/AnimationBatch.cpp
        Source/Assets/SkeletonAssetData.cpp
        Source/Assets/SkeletonBatch.cpp
        Source/Assets/MaterialBatch.cpp
        Source/Input/InputSystem.cpp
        Source/Rendering/GPUDriven/GPUDrivenBinding.cpp
        Source/Rendering/GPUDriven/GPUDrivenDataUpload.cpp
        Source/Rendering/GPUDriven/GPUDrivenRenderer.cpp
        Source/Rendering/GPUDriven/GPUDrivenRootSig.cpp
        Source/Rendering/RenderLoop.cpp
        Source/Scene/ComponentSerialization.cpp
        Source/Scene/Scene.cpp
        Source/Scene/SceneLoader.cpp
        Source/Scene/World.cpp
        Source/Utilities/DataUtilities.cpp
        Source/Utilities/MathConverter.cpp
        Source/AGameRunner.cpp
        Source/DummyGame.cpp
        Source/GameRunner.cpp
)

target_link_libraries(DZRuntime PUBLIC DenOfIz::DenOfIzGraphics flecs::flecs spdlog::spdlog fmt::fmt)