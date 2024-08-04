cmake_minimum_required(VERSION 3.19.4)

project(artnet_example CXX OBJCXX)
set(ROOT_DIR ${PROJECT_SOURCE_DIR}/../..)
include(${ROOT_DIR}/scripts/rsfw.cmake)

set(ADDON_HEADERS 
${PROJECT_SOURCE_DIR}/lib/libpcap/include
)

set(ADDON_LIBRARIES 
${PROJECT_SOURCE_DIR}/lib/libpcap/lib/libpcap.a
)

target_include_directories(${PROJECT_NAME} PRIVATE ${ADDON_HEADERS})
target_link_libraries(${PROJECT_NAME} PRIVATE ${ADDON_LIBRARIES})

