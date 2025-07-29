#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "DenOfIz::DenOfIzGraphics" for configuration "Debug"
set_property(TARGET DenOfIz::DenOfIzGraphics APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(DenOfIz::DenOfIzGraphics PROPERTIES
  IMPORTED_IMPLIB_DEBUG "${_IMPORT_PREFIX}/lib/DenOfIzGraphics.lib"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/bin/DenOfIzGraphics.dll"
  )

list(APPEND _cmake_import_check_targets DenOfIz::DenOfIzGraphics )
list(APPEND _cmake_import_check_files_for_DenOfIz::DenOfIzGraphics "${_IMPORT_PREFIX}/lib/DenOfIzGraphics.lib" "${_IMPORT_PREFIX}/bin/DenOfIzGraphics.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
