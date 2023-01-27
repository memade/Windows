project(spdlog)

set(SOURCES_DIR ${SOLUTION_ROOT_DIR}/src/${PROJECT_NAME}/)

set(SOURCES
${SOURCES_DIR}spdlog.cpp
${SOURCES_DIR}stdafx.cpp
${SOURCES_DIR}spdlog.h
${SOURCES_DIR}stdafx.h
)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

if(ENABLE_MSVC_CRT_MT)
  MSVC_RUNTIME_LIBRARY_SET_MT(${PROJECT_NAME})
endif(ENABLE_MSVC_CRT_MT)

target_include_directories(${PROJECT_NAME} 
PRIVATE 
${SOURCES_DIR}
${SPDLOG_INCLUDE_DIR}
)

# MSVC 预编译头设置
MSVC_USE_PCH(${PROJECT_NAME} ${SOURCES_DIR}stdafx.h ${SOURCES_DIR}stdafx.cpp)
#[[
target_link_directories(${PROJECT_NAME} 
PRIVATE 
${SOLUTION_ROOT_DIR}/librarys/openssl/3.0.5/$(Platform)/
)
]]


