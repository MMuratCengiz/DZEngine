add_library(DZApp STATIC)

target_include_directories(DZApp PUBLIC Include)

target_sources(DZApp PRIVATE
        Source/App.cpp
)

target_link_libraries(DZApp PUBLIC DZEditor)