cmake_minimum_required(VERSION 3.28)

if (UNIX AND NOT APPLE)
    set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
    set(CMAKE_INSTALL_RPATH "$ORIGIN")
endif ()

set(X_VCPKG_APPLOCAL_DEPS_INSTALL ON)
project(DZEngine
        VERSION 1.0.0
        DESCRIPTION ""
        LANGUAGES CXX
)

set(VCPKG_MANIFEST_DIR ${CMAKE_CURRENT_SOURCE_DIR})
include("${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake")

if (WIN32)
    list(APPEND CMAKE_PREFIX_PATH "_ThirdParty/DenOfIzGraphics/win32/")
elseif (LINUX)
    list(APPEND CMAKE_PREFIX_PATH "_ThirdParty/DenOfIzGraphics/linux/")
else ()
    list(APPEND CMAKE_PREFIX_PATH "_ThirdParty/DenOfIzGraphics/osx/")
endif ()

find_package(DenOfIzGraphics REQUIRED)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(DenOfIzGraphics REQUIRED)

add_subdirectory(Code)

set(SOURCES Main.cpp)
if (APPLE)
    set(SOURCES Main.cpp ${CMAKE_CURRENT_SOURCE_DIR}/OS/MacOS/MainMenu.xib)
endif ()

if (APPLE)
    add_executable(DZEngine MACOSX_BUNDLE ${SOURCES})
else ()
    add_executable(DZEngine ${SOURCES})
endif ()

if (APPLE)
    set_target_properties(DZEngine PROPERTIES
            BUNDLE True
            XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_WEAK YES
            MACOSX_BUNDLE_GUI_IDENTIFIER com.denofiz.DZEngine
            MACOSX_BUNDLE_EXECUTABLE_NAME DZEngine
            MACOSX_BUNDLE_BUNDLE_NAME DZEngine
            MACOSX_BUNDLE_BUNDLE_VERSION "0.1"
            MACOSX_BUNDLE_SHORT_VERSION_STRING "0.1"
            MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/OS/MacOS/MacOSBundleInfo.plist.in
            RESOURCE ${CMAKE_CURRENT_SOURCE_DIR}/OS/MacOS/MainMenu.xib
    )
    set_target_properties(DZEngine PROPERTIES XCODE_ATTRIBUTE_OTHER_CODE_SIGN_FLAGS "--deep")
    set_property(TARGET DZEngine APPEND_STRING PROPERTY COMPILE_FLAGS "-fobjc-arc")
endif ()

set_target_properties(DZEngine PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
        RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}"
)

target_link_libraries(DZEngine PRIVATE DZApp)
denofiz_setup_target(DZEngine)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/assets.cmake)
copy_to_binary_target("${CMAKE_CURRENT_SOURCE_DIR}" "_Assets" * TheKindlingScript)