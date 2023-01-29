if(MSVC)

set(VS_PLATFORM ${CMAKE_VS_PLATFORM_NAME})
set(VS_CONFIGURATION "$(Configuration)")


macro(MSVC_SET_STARTUP_PROJECT PROJECTNAME PROJECTDIR)
set_property(DIRECTORY ${PROJECTDIR} PROPERTY VS_STARTUP_PROJECT ${PROJECTNAME})
endmacro(MSVC_SET_STARTUP_PROJECT PROJECTNAME)

macro(MSVC_RUNTIME_LIBRARY_SET_MT PROJECTNAME)
set_property(TARGET ${PROJECTNAME} PROPERTY
  MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
		target_compile_options(${PROJECTNAME} PRIVATE "$<$<CONFIG:Release>:/GL>")
  target_link_options(${PROJECTNAME} PRIVATE "$<$<CONFIG:Release>:/LTCG>")
endmacro(MSVC_RUNTIME_LIBRARY_SET_MT)

macro(MSVC_RUNTIME_LIBRARY_SET_MD PROJECTNAME)
set_property(TARGET ${PROJECTNAME} PROPERTY
  MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
endmacro(MSVC_RUNTIME_LIBRARY_SET_MD)

#arg 's project name
macro(MSVC_USE_CRT_OPTION arg)
  string(TOUPPER ${arg} PROJECTNAME)
  option(ENABLE_MSVC_CRT_MT_${PROJECTNAME} "Enable project ${arg} msvc crt /MT /MTd compile." ON)
  if(ENABLE_MSVC_CRT_MT_${PROJECTNAME})
		set_property(TARGET ${arg} PROPERTY
  MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
  #add_compile_options("$<$<CONFIG:Release>:/GL>")
  #add_link_options("$<$<CONFIG:Release>:/LTCG>")
  target_compile_options(${arg} PRIVATE "$<$<CONFIG:Release>:/GL>")
  target_link_options(${arg} PRIVATE "$<$<CONFIG:Release>:/LTCG>")
  endif(ENABLE_MSVC_CRT_MT_${PROJECTNAME})
endmacro(MSVC_USE_CRT_OPTION)

#USE_MSVC_PCH        ：宏名字
#PCH_TARGET          ：项目名称
#PCH_HEADER_FILE     ：预编译头文件名称（stdafx.h）
#PCH_SOURCE_FILE     ：预编译源文件名称（stdafx.cpp）
macro(MSVC_USE_PCH PCH_TARGET PCH_HEADER_FILE PCH_SOURCE_FILE)
		# 获取预编译头文件的文件名，通常是stdafx
		get_filename_component(PCH_NAME ${PCH_HEADER_FILE} NAME_WE)
		
		# 生成预编译文件的路径
		if(CMAKE_CONFIGURATION_TYPES)
									# 如果有配置选项（Debug/Release），路径添加以及配置选项
			set(PCH_DIR "${CMAKE_CURRENT_BINARY_DIR}/PCH/${CMAKE_CFG_INTDIR}")
		else(CMAKE_CONFIGURATION_TYPES)
			set(PCH_DIR "${CMAKE_CURRENT_BINARY_DIR}/PCH")
		endif(CMAKE_CONFIGURATION_TYPES)
		# 创建预编译文件的路径
		file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/PCH)
		# 设置项目属性，使用预编译头文件
		set_target_properties(${PCH_TARGET} PROPERTIES COMPILE_FLAGS 
			"/Yu${PCH_HEADER_FILE} /FI${PCH_HEADER_FILE} /Fp${PCH_DIR}/${PCH_NAME}.pch")
		# 预编译源文件（stdafx.cpp）设置属性，创建预编译文件
		set_source_files_properties(${PCH_SOURCE_FILE} PROPERTIES COMPILE_FLAGS
			"/Yc${PCH_HEADER_FILE}")
		# 把预编译文件寄到清除列表
		set_directory_properties(PROPERTIES
			ADDITIONAL_MAKE_CLEAN_FILES ${PCH_DIR}/${PCH_NAME}.pch)
endmacro(MSVC_USE_PCH)

macro(MSVC_SET_LINK_FLAG_CONSOLE PROJECTNAME)
set_target_properties(${PROJECTNAME} PROPERTIES LINK_FLAGS "/SUBSYSTEM:CONSOLE")
endmacro(MSVC_SET_LINK_FLAG_CONSOLE)

macro(MSVC_SET_LINK_FLAG_WINDOWS PROJECTNAME)
set_target_properties(${PROJECTNAME} PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")
endmacro(MSVC_SET_LINK_FLAG_WINDOWS)









endif(MSVC)


