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

#framework
add_library(framework
        framework/application.cpp
        framework/camera.cpp
        framework/material.cpp
        framework/model.cpp
        framework/scene.cpp
        framework/shaderloader.cpp
        framework/shaderprogram.cpp
        framework/texture.cpp
        framework/uniformregistry.cpp)
target_include_directories(framework PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(framework PUBLIC cxx_std_20)
set_target_properties(framework PROPERTIES CXX_EXTENSIONS OFF)

#glsl files
file(COPY src/glsl DESTINATION ${CMAKE_BINARY_DIR})
#resource files
file(COPY src/resources DESTINATION ${CMAKE_BINARY_DIR})

add_executable(ProjectLaboratory src/main.cpp)

target_link_libraries(framework PUBLIC glfw glad OpenGL::GL glm stb_image)
target_link_libraries(ProjectLaboratory PRIVATE framework)
