add_library(DZEditor STATIC)
find_package(imgui CONFIG REQUIRED)

target_include_directories(DZEditor PUBLIC Include ImGui)

target_sources(DZEditor PRIVATE
        ImGui/ImGuiBackend.cpp
        Source/Fonts/FontAwesome.cpp
        Source/Fonts/JetbrainsMonoImGui.cpp
        Source/Editor.cpp
        Source/EditorGameRunner.cpp
        Source/Style/EditorStyle.cpp
        Source/Style/ImGuiFonts.cpp
        Source/Components/AssetBrowser.cpp
        Source/Components/SceneViewRenderer.cpp
        Source/Components/WorldEditorView.cpp
)

target_link_libraries(DZEditor PUBLIC DZRuntime imgui::imgui)