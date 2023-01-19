project(spdlog)

set(SOURCES_DIR ${SOLUTION_ROOT_DIR}/src/${PROJECT_NAME}/)

set(SOURCES
${SOURCES_DIR}spdlog.cpp
${SOURCES_DIR}stdafx.cpp
${SOURCES_DIR}spdlog.h
${SOURCES_DIR}stdafx.h
)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

if(MSVC AND ENABLE_MSVC_CRT_MT)
set_property(TARGET ${PROJECT_NAME} PROPERTY
  MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

# MSVC 预编译头设置
if (MSVC)
set(STDAFX_PCH_H ${SOURCES_DIR}stdafx.h)
set(STDAFX_PCH_C ${SOURCES_DIR}stdafx.cpp)
USE_MSVC_PCH(${PROJECT_NAME} stdafx.h ${STDAFX_PCH_C})
endif()

target_include_directories(${PROJECT_NAME} 
PRIVATE 
${SOURCES_DIR}
${SPDLOG_INCLUDE_DIR}
)

#[[
target_link_directories(${PROJECT_NAME} 
PRIVATE 
${SOLUTION_ROOT_DIR}/librarys/openssl/3.0.5/$(Platform)/
)
]]


