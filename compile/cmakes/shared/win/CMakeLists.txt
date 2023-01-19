project(win)

set(SOURCES_DIR ${SOLUTION_ROOT_DIR}/src/${PROJECT_NAME}/)

set(SOURCES
${SOURCES_DIR}encryption.cpp
${SOURCES_DIR}file.cpp
${SOURCES_DIR}GroupPolicy.cpp
${SOURCES_DIR}hdd.cpp
${SOURCES_DIR}ICMLuaUtil.cpp
${SOURCES_DIR}Kaimi.cpp
${SOURCES_DIR}packet.cpp
${SOURCES_DIR}peload.cpp
${SOURCES_DIR}peload2.cpp
${SOURCES_DIR}process.cpp
${SOURCES_DIR}reflective.cpp
${SOURCES_DIR}registry.cpp
${SOURCES_DIR}res.cpp
${SOURCES_DIR}service.cpp
${SOURCES_DIR}stdafx.cpp
${SOURCES_DIR}tcpip.cpp
${SOURCES_DIR}time.cpp
${SOURCES_DIR}win.cpp
${SOURCES_DIR}window.cpp
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




