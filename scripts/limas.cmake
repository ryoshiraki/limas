cmake_policy(SET CMP0069 NEW)

set(CMAKE_GENERATOR "Ninja")

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build (Debug or Release)" FORCE)
endif()

find_program(CCACHE_PROGRAM ccache)
if (CCACHE_PROGRAM)
    set(CMAKE_C_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
    set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
endif()

file(GLOB_RECURSE SRC_FILES 
${PROJECT_SOURCE_DIR}/src/*.cpp
${PROJECT_SOURCE_DIR}/src/*.c
${PROJECT_SOURCE_DIR}/src/*.cxx)

if (APPLE)
    add_executable(${PROJECT_NAME} MACOSX_BUNDLE ${SRC_FILES})

    set_target_properties(${PROJECT_NAME} PROPERTIES
        MACOSX_BUNDLE True
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/Info.plist
    )
    
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${FRAMEWORK_PATH}/libs/Syphon/lib/osx/Syphon.framework ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.app/Contents/Frameworks/Syphon.framework
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${FRAMEWORK_PATH}/resources ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.app/Contents/Resources/common
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
    add_executable(${PROJECT_NAME} ${SRC_FILES})
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
set(BUILD_SHARED_LIBS OFF)

# target_compile_options(${PROJECT_NAME} PRIVATE -std=c++17 -mtune=native -march=native)
target_compile_options(${PROJECT_NAME} PRIVATE -std=c++20 -w)

target_precompile_headers(${PROJECT_NAME}
PRIVATE
 ${FRAMEWORK_PATH}/libs/limas/include/pch.h
)



find_package(OpenGL REQUIRED)
find_package(glfw3 3.3 REQUIRED)

# set(GLEW_STATIC ON)
find_package(GLEW REQUIRED)

#
find_package(glm REQUIRED)

# set(Boost_USE_STATIC_LIBS ON)
find_package(Boost 1.85.0 REQUIRED COMPONENTS system iostreams filesystem)

# set(ASSIMP_BUILD_STATIC_LIB ON)
find_package(Assimp REQUIRED)

# set(FREETYPE_STATIC ON)
find_package(Freetype REQUIRED)

#
find_package(LibXml2 REQUIRED)

# set(SNAPPY_USE_STATIC_LIBS ON)
find_package(Snappy REQUIRED)

#
find_package(ZLIB REQUIRED)

# set(CGAL_USE_STATIC_LIBS ON)
find_package(CGAL REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(FFMPEG REQUIRED IMPORTED_TARGET libavcodec libavformat libavutil libswscale libswresample libavfilter)
# pkg_check_modules(LIBXML2 REQUIRED IMPORTED_TARGET libxml-2.0)

set(CORE_HEADERS
    ${OPENGL_INCLUDE_DIRS} 
    ${GLFW_INCLUDE_DIRS}
    ${GLEW_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS} 
    ${ASSIMP_INCLUDE_DIRS}
    ${FFMPEG_INCLUDE_DIRS}
    ${GLM_INCLUDE_DIRS}
    ${FREETYPE_INCLUDE_DIRS}
    ${LIBXML2_INCLUDE_DIR}
    ${Snappy_INCLUDE_DIR}

    ${PROJECT_SOURCE_DIR}/src
    ${FRAMEWORK_PATH}/libs/limas/include 
    ${FRAMEWORK_PATH}/libs/json/include
    ${FRAMEWORK_PATH}/libs/stb/include
    ${FRAMEWORK_PATH}/libs/oscpp/include #unstable
    ${FRAMEWORK_PATH}/libs/oscpack/include
    # ${FRAMEWORK_PATH}/libs/oscpack/include/osc
    # ${FRAMEWORK_PATH}/libs/oscpack/include/ip

    ${FRAMEWORK_PATH}/libs/imgui/include
    ${FRAMEWORK_PATH}/libs/FastNoiseLite/include
    ${FRAMEWORK_PATH}/libs/earcut/include
    ${FRAMEWORK_PATH}/libs/spline_library/include
    ${FRAMEWORK_PATH}/libs/bezier/include
    ${FRAMEWORK_PATH}/libs/Clipper2Lib/include
    ${FRAMEWORK_PATH}/libs/hap/src
    ${FRAMEWORK_PATH}/libs/Syphon/src
    ${FRAMEWORK_PATH}/libs/svgtiny/include
    ${FRAMEWORK_PATH}/libs/tinygltf/include
    ${FRAMEWORK_PATH}/libs/tinyexr/include
    ${FRAMEWORK_PATH}/libs/box2d/include

    # ${FRAMEWORK_PATH}/libs/NDISDK/include
    # ${FRAMEWORK_PATH}/libs/opencv/include/opencv4
    # ${FRAMEWORK_PATH}/libs/openvdb/include
    # ${FRAMEWORK_PATH}/libs/openvdb/include/openvdb
    # ${FRAMEWORK_PATH}/libs/openvdb/include/nanovdb
    # ${FRAMEWORK_PATH}/libs/tbb/include
    # ${FRAMEWORK_PATH}/libs/c-blosc/include
    # ${FRAMEWORK_PATH}/libs/libgizmo/include
)

set(CORE_SOURCES
    ${FRAMEWORK_PATH}/libs/limas/include/app/AppUtils.cpp

    ${FRAMEWORK_PATH}/libs/imgui/include/imgui.cpp
    ${FRAMEWORK_PATH}/libs/imgui/include/imgui_demo.cpp
    ${FRAMEWORK_PATH}/libs/imgui/include/imgui_tables.cpp
    ${FRAMEWORK_PATH}/libs/imgui/include/imgui_widgets.cpp
    ${FRAMEWORK_PATH}/libs/imgui/include/imgui_draw.cpp
    ${FRAMEWORK_PATH}/libs/imgui/include/imgui_draw.cpp
    ${FRAMEWORK_PATH}/libs/imgui/include/imgui_impl_opengl3.cpp
    ${FRAMEWORK_PATH}/libs/imgui/include/imgui_impl_glfw.cpp

    ${FRAMEWORK_PATH}/libs/hap/src/hap.c
    ${FRAMEWORK_PATH}/libs/Syphon/src/SyphonNameboundClient.mm
    ${FRAMEWORK_PATH}/libs/limas/include/syphon/RSSyphonClient.mm
    ${FRAMEWORK_PATH}/libs/limas/include/syphon/RSSyphonServer.mm
    ${FRAMEWORK_PATH}/libs/limas/include/syphon/RSSyphonServerDirectory.mm
    ${FRAMEWORK_PATH}/libs/limas/include/syphon/RSSyphonObject.mm

    # ${FRAMEWORK_PATH}/libs/oscpack/src/IpEndpointName.cpp
    # ${FRAMEWORK_PATH}/libs/oscpack/src/OscTypes.cpp
    # ${FRAMEWORK_PATH}/libs/oscpack/src/OscReceivedElements.cpp
    # ${FRAMEWORK_PATH}/libs/oscpack/src/OscPrintReceivedElements.cpp
    # ${FRAMEWORK_PATH}/libs/oscpack/src/OscOutboundPacketStream.cpp

    # ${FRAMEWORK_PATH}/libs/libgizmo/include/GizmoTransformMove.cpp
    # ${FRAMEWORK_PATH}/libs/libgizmo/include/GizmoTransformRender.cpp
    # ${FRAMEWORK_PATH}/libs/libgizmo/include/GizmoTransformRotate.cpp
    # ${FRAMEWORK_PATH}/libs/libgizmo/include/GizmoTransformScale.cpp
    # ${FRAMEWORK_PATH}/libs/libgizmo/include/ZBaseMaths.cpp
    # ${FRAMEWORK_PATH}/libs/libgizmo/include/ZMathsFunc.cpp
)

set_source_files_properties(${FRAMEWORK_PATH}/libs/Syphon/src/SyphonNameboundClient.mm PROPERTIES COMPILE_FLAGS "${CMAKE_OBJCXX_FLAGS}")
set_source_files_properties(${FRAMEWORK_PATH}/libs/limas/include/syphon/RSSyphonClient.mm PROPERTIES COMPILE_FLAGS "${CMAKE_OBJCXX_FLAGS}")
set_source_files_properties(${FRAMEWORK_PATH}/libs/limas/include/syphon/RSSyphonServer.mm PROPERTIES COMPILE_FLAGS "${CMAKE_OBJCXX_FLAGS}")
set_source_files_properties(${FRAMEWORK_PATH}/libs/limas/include/syphon/RSSyphonObject.mm PROPERTIES COMPILE_FLAGS "${CMAKE_OBJCXX_FLAGS}")

set(CORE_LIBRARIES ${OPENGL_LIBRARIES} GLEW::GLEW glfw
    Boost::system
    Boost::iostreams
    Boost::filesystem
    ${ASSIMP_LIBRARIES}
    ${FREETYPE_LIBRARIES}
    ${LIBXML2_LIBRARIES}
    ${Snappy_LIBRARIES}

    PkgConfig::FFMPEG

    ${FRAMEWORK_PATH}/libs/Syphon/lib/osx/Syphon.framework
    ${FRAMEWORK_PATH}/libs/svgtiny/lib/osx/svgtiny.a
    ${FRAMEWORK_PATH}/libs/tinygltf/lib/libtinygltf.a
    ${FRAMEWORK_PATH}/libs/oscpack/lib/liboscpack.a
    ${FRAMEWORK_PATH}/libs/box2d/lib/libbox2d.a

    ZLIB::ZLIB

    CGAL::CGAL
    
    # ${FRAMEWORK_PATH}/libs/NDISDK/lib/libndi.dylib //TODO: change to static library

    # ${FRAMEWORK_PATH}/libs/openvdb/lib/libopenvdb.a
    # ${FRAMEWORK_PATH}/libs/tbb/lib/libtbb.a
    # ${FRAMEWORK_PATH}/libs/tbb/lib/libtbbmalloc.a
    # ${FRAMEWORK_PATH}/libs/c-blosc/lib/libblosc.a
    # ${FRAMEWORK_PATH}/libs/opencv/lib/libopencv_core.a
    # ${FRAMEWORK_PATH}/libs/opencv/lib/libopencv_imgproc.a
)    

set(CORE_DEFINITIONS 
    GLM_ENABLE_EXPERIMENTAL
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
message(STATUS "FFmpeg include dirs: ${FFMPEG_INCLUDE_DIRS}")
message(STATUS "FFmpeg libraries: ${FFMPEG_LIBRARIES}")
message(STATUS "Assimp include dirs: ${ASSIMP_INCLUDE_DIRS}")
message(STATUS "Assimp libraries: ${ASSIMP_LIBRARIES}")