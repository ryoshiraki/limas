set(CMAKE_GENERATOR "Ninja")

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build (Debug or Release)" FORCE)
endif()

find_program(CCACHE_PROGRAM ccache)
if (CCACHE_PROGRAM)
    set(CMAKE_C_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
    set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
endif()


if (APPLE)
    add_executable(${PROJECT_NAME} MACOSX_BUNDLE ${PROJECT_SOURCE_DIR}/src/main.cpp)

    set_target_properties(${PROJECT_NAME} PROPERTIES
        MACOSX_BUNDLE True
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/Info.plist
    )
    
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${ROOT_DIR}/libs/Syphon/lib/osx/Syphon.framework ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.app/Contents/Frameworks/Syphon.framework
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${ROOT_DIR}/resources ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.app/Contents/Resources/common
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/resources ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.app/Contents/Resources
        ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.app/Contents/Frameworks/Syphon.framework
        COMMENT "Copying assets directory to .app bundle"
    )
    
    set(CORE_FRAMEWORKS
        "-framework Accelerate" 
        "-framework AGL" 
        "-framework ApplicationServices"
        "-framework AVFoundation"
        "-framework AudioToolbox"
        "-framework Cocoa"
        "-framework CoreAudio"
        "-framework CoreFoundation"
        "-framework CoreMedia"
        "-framework CoreServices"
        "-framework CoreVideo"
        "-framework IOKit"
        "-framework OpenGL"
        "-framework OpenCL"
        "-framework Security"
        "-framework VideoToolbox"
    )
else()
    add_executable(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/src/main.cpp)
endif()

# set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mtune=native -march=native")

# set(CMAKE_C_STANDARD 11)
# set(CMAKE_C_STANDARD_REQUIRED ON)
# set(CMAKE_C_COMPILER "/usr/bin/clang")
# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mtune=native -march=native")

# set(CMAKE_OBJCXX_STANDARD 17)
# set(CMAKE_OBJCXX_STANDARD_REQUIRED ON)
# set(CMAKE_OBJCXX_COMPILER "/usr/bin/clang++")
# set(CMAKE_OBJCXX_FLAGS "${CMAKE_OBJCXX_FLAGS} -mtune=native -march=native")

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++ -lc++abi")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# target_compile_options(${PROJECT_NAME} PRIVATE -std=c++17 -mtune=native -march=native)
target_compile_options(${PROJECT_NAME} PRIVATE -std=c++17)

target_precompile_headers(${PROJECT_NAME}
PRIVATE
 ${ROOT_DIR}/libs/rs/include/pch.h
)

find_package(OpenGL REQUIRED)
find_package(glfw3 3.3 REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm REQUIRED)
find_package(Boost 1.85.0 REQUIRED COMPONENTS system iostreams filesystem python312 numpy312)
find_package(Python3 REQUIRED COMPONENTS Interpreter Development)
find_package(Assimp REQUIRED)
find_package(Freetype REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(FFMPEG REQUIRED IMPORTED_TARGET libavcodec libavformat libavutil libswscale libswresample libavfilter)

set(CORE_HEADERS
    ${OPENGL_INCLUDE_DIRS} 
    ${GLFW_INCLUDE_DIRS}
    ${GLEW_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS} 
    ${Python3_INCLUDE_DIRS} 
    ${ASSIMP_INCLUDE_DIRS}
    ${FFMPEG_INCLUDE_DIRS}
    ${GLM_INCLUDE_DIRS}
    ${FREETYPE_INCLUDE_DIRS}

    ${PROJECT_SOURCE_DIR}/src
    ${ROOT_DIR}/libs/rs/include 
    ${ROOT_DIR}/libs/json/include
    ${ROOT_DIR}/libs/stb/include
    ${ROOT_DIR}/libs/oscpack/include

    ${ROOT_DIR}/libs/imgui/include
    ${ROOT_DIR}/libs/FastNoiseLite/include
    ${ROOT_DIR}/libs/earcut/include
    ${ROOT_DIR}/libs/spline_library/include
    ${ROOT_DIR}/libs/bezier/include
    ${ROOT_DIR}/libs/Clipper2Lib/include
    ${ROOT_DIR}/libs/snappy/include
    ${ROOT_DIR}/libs/hap/src
    ${ROOT_DIR}/libs/Syphon/src
    ${ROOT_DIR}/libs/svgtiny/include
    ${ROOT_DIR}/libs/libxml2/include
    ${ROOT_DIR}/libs/tinygltf/include

    # ${ROOT_DIR}/libs/NDISDK/include
    # ${ROOT_DIR}/libs/opencv/include/opencv4
    # ${ROOT_DIR}/libs/openvdb/include
    # ${ROOT_DIR}/libs/openvdb/include/openvdb
    # ${ROOT_DIR}/libs/openvdb/include/nanovdb
    # ${ROOT_DIR}/libs/tbb/include
    # ${ROOT_DIR}/libs/c-blosc/include
    # ${ROOT_DIR}/libs/libgizmo/include
)

set(CORE_SOURCES
    ${ROOT_DIR}/libs/rs/include/app/AppUtils.cpp

    ${ROOT_DIR}/libs/imgui/include/imgui.cpp
    ${ROOT_DIR}/libs/imgui/include/imgui_demo.cpp
    ${ROOT_DIR}/libs/imgui/include/imgui_tables.cpp
    ${ROOT_DIR}/libs/imgui/include/imgui_widgets.cpp
    ${ROOT_DIR}/libs/imgui/include/imgui_draw.cpp
    ${ROOT_DIR}/libs/imgui/include/imgui_draw.cpp
    ${ROOT_DIR}/libs/imgui/include/imgui_impl_opengl3.cpp
    ${ROOT_DIR}/libs/imgui/include/imgui_impl_glfw.cpp

    ${ROOT_DIR}/libs/hap/src/hap.c
    ${ROOT_DIR}/libs/Syphon/src/SyphonNameboundClient.mm
    ${ROOT_DIR}/libs/rs/include/syphon/RSSyphonClient.mm
    ${ROOT_DIR}/libs/rs/include/syphon/RSSyphonServer.mm
    ${ROOT_DIR}/libs/rs/include/syphon/RSSyphonServerDirectory.mm
    ${ROOT_DIR}/libs/rs/include/syphon/RSSyphonObject.mm

    # ${ROOT_DIR}/libs/libgizmo/include/GizmoTransformMove.cpp
    # ${ROOT_DIR}/libs/libgizmo/include/GizmoTransformRender.cpp
    # ${ROOT_DIR}/libs/libgizmo/include/GizmoTransformRotate.cpp
    # ${ROOT_DIR}/libs/libgizmo/include/GizmoTransformScale.cpp
    # ${ROOT_DIR}/libs/libgizmo/include/ZBaseMaths.cpp
    # ${ROOT_DIR}/libs/libgizmo/include/ZMathsFunc.cpp
)

set_source_files_properties(${ROOT_DIR}/libs/Syphon/src/SyphonNameboundClient.mm PROPERTIES COMPILE_FLAGS "${CMAKE_OBJCXX_FLAGS}")
set_source_files_properties(${ROOT_DIR}/libs/rs/include/syphon/RSSyphonClient.mm PROPERTIES COMPILE_FLAGS "${CMAKE_OBJCXX_FLAGS}")
set_source_files_properties(${ROOT_DIR}/libs/rs/include/syphon/RSSyphonServer.mm PROPERTIES COMPILE_FLAGS "${CMAKE_OBJCXX_FLAGS}")
set_source_files_properties(${ROOT_DIR}/libs/rs/include/syphon/RSSyphonObject.mm PROPERTIES COMPILE_FLAGS "${CMAKE_OBJCXX_FLAGS}")

set(CORE_LIBRARIES ${OPENGL_LIBRARIES} GLEW::GLEW glfw
    Boost::system
    Boost::iostreams
    Boost::filesystem
    Boost::python
    Boost::numpy
    ${Python3_LIBRARIES}
    ${ASSIMP_LIBRARIES}
    ${FREETYPE_LIBRARIES}

    PkgConfig::FFMPEG

    ${ROOT_DIR}/libs/oscpack/lib/liboscpack.a
    ${ROOT_DIR}/libs/snappy/lib/libsnappy.a
    ${ROOT_DIR}/libs/Syphon/lib/osx/Syphon.framework
    ${ROOT_DIR}/libs/svgtiny/lib/osx/svgtiny.a
    ${ROOT_DIR}/libs/libxml2/lib/osx/xml2.a
    ${ROOT_DIR}/libs/tinygltf/lib/libtinygltf.a

    # ${ROOT_DIR}/libs/NDISDK/lib/libndi.dylib //TODO: change to static library

    # ${ROOT_DIR}/libs/openvdb/lib/libopenvdb.a
    # ${ROOT_DIR}/libs/tbb/lib/libtbb.a
    # ${ROOT_DIR}/libs/tbb/lib/libtbbmalloc.a
    # ${ROOT_DIR}/libs/c-blosc/lib/libblosc.a
    # ${ROOT_DIR}/libs/opencv/lib/libopencv_core.a
    # ${ROOT_DIR}/libs/opencv/lib/libopencv_imgproc.a
)    

set(CORE_DEFINITIONS 
    GLM_ENABLE_EXPERIMENTAL
    # BOOST_PYTHON_STATIC_LIB
    BOOST_COMPUTE_USE_CPP11
    STB_IMAGE_STATIC
    STB_IMAGE_RESIZE_STATIC
    STB_IMAGE_WRITE_STATIC
    STB_IMAGE_IMPLEMENTATION
    STB_IMAGE_RESIZE_IMPLEMENTATION
    STB_IMAGE_WRITE_IMPLEMENTATION
    IMGUI_IMPL_OPENGL_LOADER_CUSTOM
    # TINYGLTF_NO_INCLUDE_STB_IMAGE
    # TINYGLTF_NO_INCLUDE_STB_IMAGE_WRITE
    # TINYGLTF_IMPLEMENTATION
)

target_sources(${PROJECT_NAME} 
PRIVATE 
    ${CORE_SOURCES}
)

target_include_directories(${PROJECT_NAME} 
PRIVATE
    ${OPENGL_INCLUDE_DIR} 
    ${CORE_HEADERS}
)

target_compile_definitions(${PROJECT_NAME} 
PRIVATE 
    ${CORE_DEFINITIONS}
)

target_link_libraries(${PROJECT_NAME} 
PRIVATE
    ${OPENGL_LIBRARIES}
    ${CORE_LIBRARIES}
    ${CORE_FRAMEWORKS}
)

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "C compiler: ${CMAKE_C_COMPILER}")
message(STATUS "Using ccache: ${CCACHE_PROGRAM}")
message(STATUS "Boost include dirs: ${Boost_INCLUDE_DIRS}")
message(STATUS "Boost libraries: ${Boost_LIBRARIES}")
message(STATUS "GLM include dirs: ${GLM_INCLUDE_DIRS}")
message(STATUS "Python3 include dirs: ${Python3_INCLUDE_DIRS}")
message(STATUS "Python3 libraries: ${Python3_LIBRARIES}")
message(STATUS "FFmpeg include dirs: ${FFMPEG_INCLUDE_DIRS}")
message(STATUS "FFmpeg libraries: ${FFMPEG_LIBRARIES}")
message(STATUS "Assimp include dirs: ${ASSIMP_INCLUDE_DIRS}")
message(STATUS "Assimp libraries: ${ASSIMP_LIBRARIES}")