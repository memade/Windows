project(update)

set(SOURCES_DIR ${SOLUTION_ROOT_DIR}/src/tools/${PROJECT_NAME}/)

set(SOURCES
${SOURCES_DIR}main.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/SUBSYSTEM:CONSOLE")

if(MSVC AND ENABLE_MSVC_CRT_MT)
set_property(TARGET ${PROJECT_NAME} PROPERTY
  MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

target_link_directories(${PROJECT_NAME} 
PRIVATE 
${LOCAL_LIBRARYS_SHARED_PATH}/$(Platform)/
${LOCAL_LIBRARYS_3RDPARTY_PATH}/$(Platform)/
${OPENSSL_LIBRARY_DIR}/$(Platform)/
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
target_include_directories(${PROJECT_NAME} 
PRIVATE 
${SOURCES_DIR}
${OPENSSL_INCLUDE_DIR}
)

target_link_directories(${PROJECT_NAME} 
PRIVATE 
${SOLUTION_ROOT_DIR}/librarys/openssl/3.0.5/$(Platform)/
)
]]


