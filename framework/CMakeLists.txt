cmake_minimum_required(VERSION 3.16)

# =========================================
# Dependencies
# =========================================

set(CMAKE_WARN_DEPRECATED OFF CACHE BOOL "" FORCE)

# OPENGL
find_package(OpenGL REQUIRED)

# GLFW
set(GLFW_BUILD_EXAMPLES    OFF CACHE INTERNAL "")
set(GLFW_BUILD_TESTS       OFF CACHE INTERNAL "")
set(GLFW_BUILD_DOCS        OFF CACHE INTERNAL "")
set(GLFW_INSTALL           OFF CACHE INTERNAL "")
set(GLFW_BUILD_SHARED_LIBS OFF CACHE INTERNAL "")

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/libs/glfw-3.4")

# GLAD
add_library(glad STATIC "${CMAKE_CURRENT_SOURCE_DIR}/libs/glad/src/glad.c")
target_include_directories(glad PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/libs/glad/include")

# GLM
add_library(glm INTERFACE)
target_include_directories(glm INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/libs/glm")

# STB_IMAGE
add_library(stb_image STATIC "${CMAKE_CURRENT_SOURCE_DIR}/libs/stb_image/src/stb_image.cpp")
target_include_directories(stb_image PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/libs/stb_image/include")

# =========================================
# Framework library
# =========================================

# Source files
add_library(framework STATIC
        src/core/camera.cpp
        src/core/model.cpp

        src/opengl/loader/shaderloader.cpp
        src/opengl/loader/texture.cpp

        src/opengl/render/framebuffer.cpp
        src/opengl/render/framebuffercube.cpp
        src/opengl/render/rendertexture.cpp

        src/opengl/application.cpp
        src/opengl/shaderprogram.cpp

        src/scene/scene.cpp

        src/threading/jobpool.cpp
        src/threading/threadpool.cpp

        src/uniform/uniformregistry.cpp
        src/uniform/uniformsource.cpp
)

# Framework properties
set_target_properties(framework PROPERTIES
        CXX_EXTENSIONS OFF
        UNITY_BUILD ON
)

target_compile_features(framework PUBLIC cxx_std_20)

# Shader and resource paths need to be defined
if (NOT DEFINED GLSL_DIR)
    message(FATAL_ERROR
            "\n[Fatal Framework Error]: Shader path \"GLSL_DIR\" not defined!"
            "\nUse the following command to define a unique shader path:\n"
            "set(GLSL_DIR \"\${CMAKE_CURRENT_SOURCE_DIR}/your_glsl_dir/\")"
    )
endif ()

if(NOT DEFINED RESOURCES_DIR)
    message(FATAL_ERROR
            "\n[Fatal Framework Error]: Resources path \"RESOURCES_DIR\" not defined!"
            "\nUse the following command to define a unique resources path:\n"
            "set(RESOURCES_DIR \"\${CMAKE_CURRENT_SOURCE_DIR}/your_resources_dir/\")"
    )
endif ()

# Generate config.h file in binary
configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/src/config.h.in"
        "${CMAKE_CURRENT_BINARY_DIR}/include/config.h"
)

# Framework includes
target_include_directories(framework PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/.."
        "${CMAKE_CURRENT_BINARY_DIR}/include"
)

# Framework library linking
target_link_libraries(framework PUBLIC
        glfw
        glad
        OpenGL::GL
        glm
        stb_image
)