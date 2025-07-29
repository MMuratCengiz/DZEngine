#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "DenOfIz::DenOfIzGraphics" for configuration "Release"
set_property(TARGET DenOfIz::DenOfIzGraphics APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(DenOfIz::DenOfIzGraphics PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libDenOfIzGraphics.dylib"
  IMPORTED_SONAME_RELEASE "@rpath/libDenOfIzGraphics.dylib"
  )

list(APPEND _cmake_import_check_targets DenOfIz::DenOfIzGraphics )
list(APPEND _cmake_import_check_files_for_DenOfIz::DenOfIzGraphics "${_IMPORT_PREFIX}/lib/libDenOfIzGraphics.dylib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
