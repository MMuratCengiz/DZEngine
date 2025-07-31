add_library(DZEditor STATIC)

target_include_directories(DZEditor PUBLIC Include)

target_sources(DZEditor PRIVATE
        Source/Editor.cpp
        Source/EditorGameRunner.cpp
)

target_link_libraries(DZEditor PUBLIC DZRuntime)