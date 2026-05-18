cmake_minimum_required(VERSION 3.16)

project(ProjectLaboratory LANGUAGES C CXX)

# =========================================
# Dependencies
# =========================================

# glfw
add_subdirectory(libs/glfw-3.4)

# glad
add_library(glad STATIC libs/glad/src/glad.c)
target_include_directories(glad PUBLIC libs/glad/include)

# glm
add_library(glm INTERFACE)
target_include_directories(glm INTERFACE libs/glm)

# stb_image
add_library(stb_image STATIC libs/stb_image/src/stb_image.cpp)
target_include_directories(stb_image PUBLIC libs/stb_image/include)

# OpenGL
find_package(OpenGL REQUIRED)

# =========================================
# Assimp
# =========================================

set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_SAMPLES OFF CACHE BOOL "" FORCE)

set(ASSIMP_BUILD_DRACO OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_IFC_IMPORTER OFF CACHE BOOL "" FORCE)

set(ASSIMP_BUILD_ZLIB ON CACHE BOOL "" FORCE)

set(ASSIMP_NO_EXPORT ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF CACHE BOOL "" FORCE)

set(ASSIMP_BUILD_OBJ_IMPORTER ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_FBX_IMPORTER ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_GLTF_IMPORTER ON CACHE BOOL "" FORCE)

set(ASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT OFF CACHE BOOL "" FORCE)

add_subdirectory(libs/assimp)

# =========================================
# Framework library
# =========================================

add_library(framework
        framework/core/camera.cpp
        framework/core/model.cpp

        framework/opengl/loader/shaderloader.cpp
        framework/opengl/loader/texture.cpp

        framework/opengl/render/framebuffer.cpp
        framework/opengl/render/framebuffercube.cpp
        framework/opengl/render/rendertexture.cpp

        framework/opengl/application.cpp
        framework/opengl/shaderprogram.cpp

        framework/scene/scene.cpp

        framework/threading/jobpool.cpp
        framework/threading/threadpool.cpp

        framework/uniform/uniformregistry.cpp
        framework/uniform/uniformsource.cpp
)

target_include_directories(framework PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(framework PUBLIC cxx_std_20)

set_target_properties(framework PROPERTIES
        CXX_EXTENSIONS OFF
        UNITY_BUILD ON
)

target_link_libraries(framework PUBLIC
        glfw
        glad
        OpenGL::GL
        glm
        stb_image
)

# =========================================
# Executable
# =========================================

add_executable(ProjectLaboratory
        src/main.cpp
        src/code/loader/assimpmodel.cpp
        src/code/scene/builder/builder3d.cpp
        src/code/hdr/hdrtexture.cpp
)

target_compile_features(ProjectLaboratory PRIVATE cxx_std_20)

set_target_properties(ProjectLaboratory PROPERTIES
        CXX_EXTENSIONS OFF
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

target_link_libraries(ProjectLaboratory PRIVATE
        framework
        assimp::assimp
)

# =========================================
# Resource copy
# =========================================

add_custom_command(TARGET ProjectLaboratory POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/src/code/glsl
        $<TARGET_FILE_DIR:ProjectLaboratory>/glsl
)

add_custom_command(TARGET ProjectLaboratory POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/src/resources
        $<TARGET_FILE_DIR:ProjectLaboratory>/resources
)

# =========================================
# LTO/IPO Support
# =========================================

include(CheckIPOSupported)

check_ipo_supported(RESULT ipo_supported)

if(ipo_supported)
    set_property(TARGET framework PROPERTY INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
    set_property(TARGET ProjectLaboratory PROPERTY INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
endif()

# =========================================
# CPU-native optimization
# =========================================

if(NOT MSVC)
    target_compile_options(framework PRIVATE
            $<$<CONFIG:Release>:-march=native>
    )
endif()