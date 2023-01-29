project(testhttpparser)

set(SOURCES_DIR ${SOLUTION_ROOT_DIR}/tests/${PROJECT_NAME}/)

set(SOURCES
${SOURCES_DIR}main.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

MSVC_SET_LINK_FLAG_CONSOLE(${PROJECT_NAME})

if(ENABLE_MSVC_CRT_MT)
MSVC_RUNTIME_LIBRARY_SET_MT(${PROJECT_NAME})
endif(ENABLE_MSVC_CRT_MT)

target_include_directories(${PROJECT_NAME} 
PRIVATE 
${SOURCES_DIR}
${SOLUTION_ROOT_DIR}/3rdparty/http-parser
)

target_link_directories(${PROJECT_NAME} 
PRIVATE 
${LOCAL_LIBRARYS_SHARED_PATH}/${VS_PLATFORM}
${LOCAL_LIBRARYS_3RDPARTY_PATH}/${VS_PLATFORM}
)

#[[
# MSVC 预编译头设置
if (MSVC)
set(STDAFX_PCH_H ${SOURCES_DIR}stdafx.h)
set(STDAFX_PCH_C ${SOURCES_DIR}stdafx.cpp)
USE_MSVC_PCH(${PROJECT_NAME} stdafx.h ${STDAFX_PCH_C})
endif()
]]

#[[


target_link_directories(${PROJECT_NAME} 
PRIVATE 
${SOLUTION_ROOT_DIR}/librarys/openssl/3.0.5/$(Platform)/
)
]]


