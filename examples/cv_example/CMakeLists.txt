cmake_minimum_required(VERSION 3.5)

project(python_example CXX)
set(ROOT_DIR ${PROJECT_SOURCE_DIR}/../..)
include(${ROOT_DIR}/scripts/rsfw.cmake)

set(ADDON_HEADERS "")
set(ADDON_LIBRARIES "")
target_include_directories(${PROJECT_NAME} PRIVATE ${ADDON_HEADERS})
target_link_libraries(${PROJECT_NAME} PRIVATE ${ADDON_LIBRARIES})

# set(PYTHON_HEADER "/Users/rsmacmini2018/anaconda3/envs/test/include/python3.10")
# set(PYTHON_LIBRARY "/Users/rsmacmini2018/anaconda3/envs/test/lib/libpython3.10.dylib")
# target_include_directories(${PROJECT_NAME} PRIVATE ${PYTHON_HEADER})
# target_link_libraries(${PROJECT_NAME} PRIVATE ${PYTHON_LIBRARY})

set(PYTHON_HEADER "/Users/rs/opt/anaconda3/envs/rs/include/python3.11")
set(PYTHON_LIBRARY "/Users/rs/opt/anaconda3/envs/rs/lib/libpython3.11.dylib")
target_include_directories(${PROJECT_NAME} PRIVATE ${PYTHON_HEADER})
target_link_libraries(${PROJECT_NAME} PRIVATE ${PYTHON_LIBRARY})
 