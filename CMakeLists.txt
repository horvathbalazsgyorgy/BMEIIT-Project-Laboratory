cmake_minimum_required(VERSION 3.20)
project(ProjectLaboratory LANGUAGES C CXX)

#glfw
add_subdirectory(libs/glfw-3.4)

#glad
add_library(glad STATIC libs/glad/src/glad.c)
target_include_directories(glad PUBLIC libs/glad/include)

#glm
add_library(glm INTERFACE)
target_include_directories(glm INTERFACE libs/glm)

#stb_image
add_library(stb_image STATIC libs/stb_image/src/stb_image.cpp)
target_include_directories(stb_image PUBLIC libs/stb_image/include)

#opengl
find_package(OpenGL REQUIRED)

#assimp options
set(ASSIMP_INSTALL OFF CACHE BOOL "Build only in this project" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build static library" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "Tests deleted" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "Tools deleted" FORCE)
set(ASSIMP_BUILD_SAMPLES OFF CACHE BOOL "Samples deleted" FORCE)
set(ASSIMP_BUILD_DRACO OFF CACHE BOOL "Do not need Draco compression" FORCE)
set(ASSIMP_BUILD_IFC_IMPORTER OFF CACHE BOOL "do not need architectural models" FORCE)
set(ASSIMP_BUILD_ZLIB ON CACHE BOOL "Build our own zlib" FORCE)

#assimp
add_subdirectory(libs/assimp)

#framework
add_library(framework
        framework/core/camera.cpp
        framework/core/model.cpp
        framework/opengl/loader/shaderloader.cpp
        framework/opengl/loader/texture.cpp
        framework/opengl/render/framebuffer.cpp
        framework/opengl/render/rendertexture.cpp
        framework/opengl/application.cpp
        framework/opengl/shaderprogram.cpp
        framework/scene/scene.cpp
        framework/uniform/uniformregistry.cpp
        framework/uniform/uniformsource.cpp)
target_include_directories(framework PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(framework PUBLIC cxx_std_20)
set_target_properties(framework PROPERTIES CXX_EXTENSIONS OFF)

#glsl files
file(COPY src/glsl DESTINATION ${CMAKE_BINARY_DIR})
#resource files
file(COPY src/resources DESTINATION ${CMAKE_BINARY_DIR})

add_executable(ProjectLaboratory
        src/main.cpp
        src/loader/assimpmodel.cpp
        src/test/builder3d.cpp)

target_link_libraries(framework PUBLIC glfw glad OpenGL::GL glm stb_image)
target_link_libraries(ProjectLaboratory PRIVATE framework assimp::assimp)
