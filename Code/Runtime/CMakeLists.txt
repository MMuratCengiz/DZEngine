find_package(flecs CONFIG REQUIRED)

add_library(DZRuntime STATIC)

target_include_directories(DZRuntime PUBLIC Include)

target_sources(DZRuntime PRIVATE
        Source/Rendering/RenderLoop.cpp
)

target_link_libraries(DZRuntime PUBLIC DenOfIz::DenOfIzGraphics flecs::flecs)