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

if(ENABLE_MSVC_CRT_MT)
  MSVC_RUNTIME_LIBRARY_SET_MT(${PROJECT_NAME})
endif(ENABLE_MSVC_CRT_MT)

# MSVC 预编译头设置
MSVC_USE_PCH(${PROJECT_NAME} ${SOURCES_DIR}stdafx.h ${SOURCES_DIR}stdafx.cpp)




