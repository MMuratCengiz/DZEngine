
####### Expanded from @PACKAGE_INIT@ by configure_package_config_file() #######
####### Any changes to this file will be overwritten by the next CMake run ####
####### The input file was DenOfIzGraphicsConfig.cmake.in                            ########

get_filename_component(PACKAGE_PREFIX_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../" ABSOLUTE)

macro(set_and_check _var _file)
  set(${_var} "${_file}")
  if(NOT EXISTS "${_file}")
    message(FATAL_ERROR "File or directory ${_file} referenced by variable ${_var} does not exist !")
  endif()
endmacro()

macro(check_required_components _NAME)
  foreach(comp ${${_NAME}_FIND_COMPONENTS})
    if(NOT ${_NAME}_${comp}_FOUND)
      if(${_NAME}_FIND_REQUIRED_${comp})
        set(${_NAME}_FOUND FALSE)
      endif()
    endif()
  endforeach()
endmacro()

####################################################################################

include("${CMAKE_CURRENT_LIST_DIR}/DenOfIzGraphicsTargets.cmake")
if(WIN32)
    set(DENOFIZ_RUNTIME_DIR "${PACKAGE_PREFIX_DIR}/bin")
else()
    set(DENOFIZ_RUNTIME_DIR "${PACKAGE_PREFIX_DIR}/lib")
endif()

get_target_property(DENOFIZ_COMPILE_DEFS DenOfIz::DenOfIzGraphics INTERFACE_COMPILE_DEFINITIONS)
if("BUILD_WEBGPU_NATIVE" IN_LIST DENOFIZ_COMPILE_DEFS)
    set(DENOFIZ_HAS_WEBGPU_NATIVE TRUE)
else()
    set(DENOFIZ_HAS_WEBGPU_NATIVE FALSE)
endif()

if (WIN32)
    set_property(TARGET DenOfIz::DenOfIzGraphics APPEND PROPERTY
        INTERFACE_LINK_LIBRARIES dxgi d3d12 dxcompiler
    )
    set_property(TARGET DenOfIz::DenOfIzGraphics APPEND PROPERTY
        INTERFACE_COMPILE_DEFINITIONS BUILD_DX12 BUILD_VK TVG_STATIC
    )
    if(MSVC)
        set_property(TARGET DenOfIz::DenOfIzGraphics APPEND PROPERTY
            INTERFACE_COMPILE_OPTIONS "/MT$<$<CONFIG:Debug>:d>"
        )
    endif()
elseif (APPLE)
    set_property(TARGET DenOfIz::DenOfIzGraphics APPEND PROPERTY
        INTERFACE_LINK_LIBRARIES 
        "-framework Cocoa"
        "-framework Foundation"
        "-framework Metal"
        "-framework MetalKit"
        dxcompiler
    )
    set_property(TARGET DenOfIz::DenOfIzGraphics APPEND PROPERTY
        INTERFACE_COMPILE_DEFINITIONS BUILD_METAL TVG_STATIC
    )
    set_property(TARGET DenOfIz::DenOfIzGraphics APPEND PROPERTY
        INTERFACE_COMPILE_OPTIONS
        "$<$<COMPILE_LANGUAGE:CXX>:-x objective-c++>"
        "$<$<COMPILE_LANGUAGE:C>:-x objective-c>"
        "-fobjc-arc"
    )
elseif (LINUX)
    set_property(TARGET DenOfIz::DenOfIzGraphics APPEND PROPERTY
        INTERFACE_LINK_LIBRARIES dxcompiler
    )
    set_property(TARGET DenOfIz::DenOfIzGraphics APPEND PROPERTY
        INTERFACE_COMPILE_DEFINITIONS BUILD_VK TVG_STATIC
    )
endif()

set_property(TARGET DenOfIz::DenOfIzGraphics APPEND PROPERTY
    INTERFACE_COMPILE_DEFINITIONS DZ_GRAPHICS_IMPORTS
)

set(DENOFIZ_GRAPHICS_INCLUDE_DIR "${PACKAGE_PREFIX_DIR}/include")
set(DENOFIZ_GRAPHICS_LIB_DIR "${PACKAGE_PREFIX_DIR}/lib")
set(DENOFIZ_GRAPHICS_RUNTIME_DIR "${DENOFIZ_RUNTIME_DIR}")

function(denofiz_copy_runtime_dependencies target)
    if(WIN32)
        set(RUNTIME_DEPS
            "${DENOFIZ_RUNTIME_DIR}/DenOfIzGraphics.dll"
            "${DENOFIZ_RUNTIME_DIR}/dxcompiler.dll"
            "${DENOFIZ_RUNTIME_DIR}/dxil.dll"
            "${DENOFIZ_RUNTIME_DIR}/metalirconverter.dll"
        )
        if(DENOFIZ_HAS_WEBGPU_NATIVE)
            list(APPEND RUNTIME_DEPS "${DENOFIZ_RUNTIME_DIR}/wgpu_native.dll")
        endif()
    elseif(APPLE)
        set(RUNTIME_DEPS
            "${DENOFIZ_RUNTIME_DIR}/libDenOfIzGraphics.dylib"
            "${DENOFIZ_RUNTIME_DIR}/libdxcompiler.dylib"
            "${DENOFIZ_RUNTIME_DIR}/libmetalirconverter.dylib"
        )
        if(DENOFIZ_HAS_WEBGPU_NATIVE)
            list(APPEND RUNTIME_DEPS "${DENOFIZ_RUNTIME_DIR}/libwgpu_native.dylib")
        endif()
    elseif(LINUX)
        set(RUNTIME_DEPS
            "${DENOFIZ_RUNTIME_DIR}/libDenOfIzGraphics.so"
            "${DENOFIZ_RUNTIME_DIR}/libdxcompiler.so"
            "${DENOFIZ_RUNTIME_DIR}/libdxil.so"
        )
        if(DENOFIZ_HAS_WEBGPU_NATIVE)
            list(APPEND RUNTIME_DEPS "${DENOFIZ_RUNTIME_DIR}/libwgpu_native.so")
        endif()
    endif()
    foreach(dep ${RUNTIME_DEPS})
        if(EXISTS ${dep})
            add_custom_command(TARGET ${target} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    ${dep} $<TARGET_FILE_DIR:${target}>
            )
        else()
            message(WARNING "Runtime dependency not found: ${dep}")
        endif()
    endforeach()
endfunction()

include("${CMAKE_CURRENT_LIST_DIR}/DenOfIzGraphicsUtils.cmake")
check_required_components(DenOfIzGraphics)
