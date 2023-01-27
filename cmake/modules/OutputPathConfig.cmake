#argv#	#是一个下标，0指向第一个参数，累加
#argv	所有的定义时要求传入的参数
#argn	定义时要求传入的参数以外的参数
#argc	传入的实际参数的个数，也就是调用函数是传入的参数个数
#argv0 LibraryFName
#argv1 RuntimeFName
#argv2 ArchiveFName
macro(OutputPathConfig arg)
if (CMAKE_CL_64)
set(COMPILE_PLATFORM x64)
else(CMAKE_CL_64)
set(COMPILE_PLATFORM x86)
endif(CMAKE_CL_64)

set(LibraryFName "bin")
set(RuntimeFName "bin")
set(ArchiveFName "librarys")
set(bSpecifiedPath OFF)

if(NOT(ARGV0 STREQUAL ""))
set(LibraryFName ${ARGV0})
elseif(ARGV0 STREQUAL "")
set(LibraryFName "")
endif(NOT(ARGV0 STREQUAL ""))

if(${ARGC} GREATER 1)
set(RuntimeFName ${ARGV1})
endif(${ARGC} GREATER 1)
if(${ARGC} GREATER 2)
set(ArchiveFName ${ARGV2})
endif(${ARGC} GREATER 2)

if(${ARGC} LESS 2)
set(RuntimeFName "")
endif(${ARGC} LESS 2)

if(${ARGC} LESS 3)
set(ArchiveFName "")
endif(${ARGC} LESS 3)

if(${ARGC} GREATER 3)
set(bSpecifiedPath ON)
endif(${ARGC} GREATER 3)

set(PUBLISH_LIBRARY ${CMAKE_CURRENT_SOURCE_DIR}/${LibraryFName})
set(PUBLISH_RUNTIME ${CMAKE_CURRENT_SOURCE_DIR}/${RuntimeFName})
set(PUBLISH_ARCHIVE ${CMAKE_CURRENT_SOURCE_DIR}/${ArchiveFName})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PUBLISH_LIBRARY})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PUBLISH_RUNTIME})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PUBLISH_ARCHIVE})

if(bSpecifiedPath)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${PUBLISH_LIBRARY})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${PUBLISH_LIBRARY})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PUBLISH_RUNTIME})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PUBLISH_RUNTIME})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${PUBLISH_ARCHIVE})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${PUBLISH_ARCHIVE})
#message(STATUS "bSpecifiedPath is set ON.")
else(bSpecifiedPath)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${PUBLISH_LIBRARY}/${COMPILE_PLATFORM}/Debug)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${PUBLISH_LIBRARY}/${COMPILE_PLATFORM}/Release)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PUBLISH_RUNTIME}/${COMPILE_PLATFORM}/Debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PUBLISH_RUNTIME}/${COMPILE_PLATFORM}/Release)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${PUBLISH_ARCHIVE}/${COMPILE_PLATFORM}/Debug)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${PUBLISH_ARCHIVE}/${COMPILE_PLATFORM}/Release)
endif(bSpecifiedPath)

#message(STATUS "CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")
#message(STATUS "CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
#message(STATUS "CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}")
#[[
message(STATUS "LibraryFName is ${LibraryFName}")
message(STATUS "RuntimeFName is ${RuntimeFName}")
message(STATUS "ArchiveFName is ${ArchiveFName}")
message(STATUS "bSpecifiedPath is ${bSpecifiedPath}")
message(STATUS "ARGC is ${ARGC}.")
message(STATUS "LibraryFName is ${LibraryFName}")
message(STATUS "RuntimeFName is ${RuntimeFName}")
message(STATUS "ArchiveFName is ${ArchiveFName}")
message(STATUS "bSpecifiedPath is ${bSpecifiedPath}")
]]
endmacro()
