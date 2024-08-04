cmake_minimum_required(VERSION 3.5)

project(ndi_example LANGUAGES CXX OBJCXX)
set(ROOT_DIR ${PROJECT_SOURCE_DIR}/../..)
include(${ROOT_DIR}/scripts/rsfw.cmake)

include(${ROOT_DIR}/addons/NDI/addon.cmake)
