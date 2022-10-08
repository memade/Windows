#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "uv_a" for configuration "Release"
set_property(TARGET uv_a APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(uv_a PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/uv_a.lib"
  )

list(APPEND _cmake_import_check_targets uv_a )
list(APPEND _cmake_import_check_files_for_uv_a "${_IMPORT_PREFIX}/lib/uv_a.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
