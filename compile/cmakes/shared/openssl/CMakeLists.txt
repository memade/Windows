project(openssl)

set(SOURCES_DIR ${SOLUTION_ROOT_DIR}/src/${PROJECT_NAME}/)

set(SOURCES
${SOURCES_DIR}business.cpp
${SOURCES_DIR}openssl.cpp
${SOURCES_DIR}stdafx.cpp
${SOURCES_DIR}stdafx.h
)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

if(ENABLE_MSVC_CRT_MT)
  MSVC_RUNTIME_LIBRARY_SET_MT(${PROJECT_NAME})
endif(ENABLE_MSVC_CRT_MT)

# MSVC 预编译头设置
MSVC_USE_PCH(${PROJECT_NAME} ${SOURCES_DIR}stdafx.h ${SOURCES_DIR}stdafx.cpp)

target_include_directories(${PROJECT_NAME} 
PRIVATE 
${SOURCES_DIR}
${OPENSSL_INCLUDE_DIR}
)

target_link_directories(${PROJECT_NAME} 
PRIVATE 
${OPENSSL_LIBRARY_DIR}/$(Platform)/
)



