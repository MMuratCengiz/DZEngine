add_library(DZCommon STATIC)

target_include_directories(DZCommon PUBLIC Include)

target_sources(DZCommon PRIVATE
        Source/GraphicsContext.cpp
)

target_link_libraries(DZCommon PUBLIC DenOfIz::DenOfIzGraphics)