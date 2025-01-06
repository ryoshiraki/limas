cmake_minimum_required(VERSION 3.5)

project(template CXX OBJCXX)
set(ROOT_DIR ${PROJECT_SOURCE_DIR}/../../..)
include(${ROOT_DIR}/scripts/limas.cmake)

# set(PYTHON_HEADER "/Users/rs/opt/anaconda3/envs/rs/include/python3.11")
# set(PYTHON_LIBRARY "/Users/rs/opt/anaconda3/envs/rs/lib/libpython3.11.dylib")
# target_include_directories(${PROJECT_NAME} PRIVATE ${PYTHON_HEADER})
# target_link_libraries(${PROJECT_NAME} PRIVATE ${PYTHON_LIBRARY})
