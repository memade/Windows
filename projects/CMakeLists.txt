# Discover the projects that use CMake in the subdirectories.
# Note that explicit cmake invocation is required every time a new project is
# added or removed.
file(GLOB entries *)
foreach(entry ${entries})
 if(IS_DIRECTORY ${entry} AND EXISTS ${entry}/CMakeLists.txt)
  get_filename_component(entry_name "${entry}" NAME)
  if(
  (entry_name STREQUAL libuvpp) 
  OR 
  (entry_name STREQUAL libcurlpp)
  OR
  (entry_name STREQUAL proxy))
  #OutputPathConfig("bin" "bin" "lib")
  #get_filename_component(entry_name "${entry}" NAME)
  #add_llvm_external_project(${entry_name})
  add_subdirectory(${entry})
  set_target_properties(${entry_name} PROPERTIES FOLDER "projects")
  message(STATUS "append project - ${entry_name}")
  #OutputPathConfig("")
  endif()
  #message(STATUS ${entry_name})
 endif()
endforeach(entry)
