cmake_minimum_required(VERSION 3.5)

project(gltf_example CXX)
set(ROOT_DIR ${PROJECT_SOURCE_DIR}/../..)
include(${ROOT_DIR}/scripts/rsfw.cmake)

set(ADDON_HEADERS "")
set(ADDON_LIBRARIES "")
target_include_directories(${PROJECT_NAME} PRIVATE ${ADDON_HEADERS})
target_link_libraries(${PROJECT_NAME} PRIVATE ${ADDON_LIBRARIES})

