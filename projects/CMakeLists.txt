# Discover the projects that use CMake in the subdirectories.
# Note that explicit cmake invocation is required every time a new project is
# added or removed.
file(GLOB entries *)
foreach(entry ${entries})
 if(IS_DIRECTORY ${entry} AND EXISTS ${entry}/CMakeLists.txt)
  get_filename_component(origin_entry_name "${entry}" NAME)
  string(TOUPPER ${origin_entry_name} upper_entry_name)
  if(
  (${upper_entry_name} STREQUAL "WINUI") 
  OR 
		(${upper_entry_name} STREQUAL "DETOURS")	
		OR
  (${upper_entry_name} STREQUAL "LIBUVPP") 
  OR 
  (${upper_entry_name} STREQUAL "LIBCURLPP")
  OR
  (${upper_entry_name} STREQUAL "PROXY"))
  add_subdirectory(${entry})
  set_target_properties(${origin_entry_name} PROPERTIES FOLDER "projects")
  elseif(${upper_entry_name} STREQUAL "MT5PROJECTS")
  #[[
  add_subdirectory(${entry})
  set_target_properties(${origin_entry_name} PROPERTIES FOLDER "projects")
  ]]
  #MT项目需要单独配置CMAKE操作
  #message(STATUS "append project - ${origin_entry_name}")
  elseif()

  
  endif()
  #message(STATUS ${entry_name})
 endif()
endforeach(entry)
