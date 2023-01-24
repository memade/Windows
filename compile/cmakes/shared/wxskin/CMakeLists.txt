project(wxskin)

set(SOURCES_DIR ${SOLUTION_ROOT_DIR}/src/${PROJECT_NAME}/)

set(SOURCES
${SOURCES_DIR}app.cpp
${SOURCES_DIR}frame.cpp
${SOURCES_DIR}frameskin.cpp
${SOURCES_DIR}mdiparentframe.cpp
${SOURCES_DIR}misc.cpp
${SOURCES_DIR}stdafx.cpp
${SOURCES_DIR}wxskin.cpp
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
${LOCAL_COMMON_INCLUDE_PATH}/wxwidgets/${PLATFORM}/
${LCOAL_PROJECTS_3RDPARTY_PATH}/wxWidgets-3.2.1/include
)

#[[
target_link_directories(${PROJECT_NAME} 
PRIVATE 
${SOLUTION_ROOT_DIR}/librarys/openssl/3.0.5/$(Platform)/
)
]]


