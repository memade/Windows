cmake_minimum_required(VERSION 3.25.1)

project (Windows)

set(CMAKE_CXX_STANDARD 23)
# 添加预定义宏
add_definitions(-DUNICODE -D_UNICODE)

# 设置默认编译类型
set(CMAKE_CONFIGURATION_TYPES "Debug;Release;" CACHE STRING "info" FORCE)

set(SOLUTION_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
#cmake modules path
set(LOCAL_CMAKE_MODULES_PATH ${SOLUTION_ROOT_DIR}/cmake/modules/)
#local common include path
set(LOCAL_COMMON_INCLUDE_PATH ${SOLUTION_ROOT_DIR}/include/)
#local projects path
set(LOCAL_PROJECTS_PATH ${SOLUTION_ROOT_DIR}/projects/)
#local runtimes path
set(LOCAL_PROJECTS_PATH ${SOLUTION_ROOT_DIR}/runtimes/)
#local src path
set(LOCAL_SRC_PATH ${SOLUTION_ROOT_DIR}/src/)
#local tools path
set(LOCAL_SRC_PATH ${SOLUTION_ROOT_DIR}/tools/bin/)
#local projects 3rdparty library path
set(LCOAL_PROJECTS_3RDPARTY_PATH ${SOLUTION_ROOT_DIR}/3rdparty/)
#spdlog include dir
set(SPDLOG_INCLUDE_DIR ${LCOAL_PROJECTS_3RDPARTY_PATH}/spdlog/include/)
#openssl 3 dir or 
#--- "${LOCAL_COMMON_INCLUDE_PATH}/openssl/1.0.2u/"
#--- "${LOCAL_COMMON_INCLUDE_PATH}/openssl/3.0.5/"
set(OPENSSL_INCLUDE_DIR ${LOCAL_COMMON_INCLUDE_PATH}/openssl/3.0.5/)

#Project generic header definition
include_directories(${LOCAL_SRC_PATH})
include_directories(${LOCAL_COMMON_INCLUDE_PATH})
include_directories(${LCOAL_PROJECTS_3RDPARTY_PATH})

if(MSVC)
option(ENABLE_MSVC_CRT_MT "Enable msvc crt /MT /MTd compile." ON)
if(ENABLE_MSVC_CRT_MT)
add_compile_options("$<$<CONFIG:Release>:/GL>")
add_link_options("$<$<CONFIG:Release>:/LTCG>")
endif(ENABLE_MSVC_CRT_MT)
endif(MSVC)

LIST(APPEND CMAKE_MODULE_PATH ${LOCAL_CMAKE_MODULES_PATH})
include(msvcpch)
include(OutputPathConfig)

OutputPathConfig("")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMakeTargets")

add_subdirectory(${SOLUTION_ROOT_DIR}/compile/cmakes/shared/win)
set_target_properties(win PROPERTIES FOLDER "shared")
add_subdirectory(${SOLUTION_ROOT_DIR}/compile/cmakes/shared/zip++)
set_target_properties(zip++ PROPERTIES FOLDER "shared")
add_subdirectory(${SOLUTION_ROOT_DIR}/compile/cmakes/shared/encryption)
set_target_properties(encryption PROPERTIES FOLDER "shared")
add_subdirectory(${SOLUTION_ROOT_DIR}/compile/cmakes/shared/iconv)
set_target_properties(iconv PROPERTIES FOLDER "shared")
add_subdirectory(${SOLUTION_ROOT_DIR}/compile/cmakes/shared/ntobj)
set_target_properties(ntobj PROPERTIES FOLDER "shared")
add_subdirectory(${SOLUTION_ROOT_DIR}/compile/cmakes/shared/openssl)
set_target_properties(openssl PROPERTIES FOLDER "shared")
add_subdirectory(${SOLUTION_ROOT_DIR}/compile/cmakes/shared/reflective)
set_target_properties(reflective PROPERTIES FOLDER "shared")
add_subdirectory(${SOLUTION_ROOT_DIR}/compile/cmakes/shared/spdlog)
set_target_properties(spdlog PROPERTIES FOLDER "shared")
add_subdirectory(${SOLUTION_ROOT_DIR}/compile/cmakes/shared/wxskin)
set_target_properties(wxskin PROPERTIES FOLDER "shared")

message(${LOCAL_SRC_PATH})
message(${LOCAL_COMMON_INCLUDE_PATH})
message(${LCOAL_PROJECTS_3RDPARTY_PATH})
message(${PUBLISH_LIB_RELEASE})
message(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})