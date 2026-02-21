cmake_minimum_required(VERSION 3.10)
project(ProjectLaboratory)

set(CMAKE_CXX_STANDARD 17)

#glfw
add_subdirectory(libs/glfw-3.4)

#glad
add_library(glad libs/glad/src/glad.c)
target_include_directories(glad PUBLIC libs/glad/include)

#glm
add_library(glm INTERFACE)
target_include_directories(glm INTERFACE libs/glm)

#opengl
find_package(OpenGL REQUIRED)

#framework
add_library(framework
        framework/framework.h
        framework/application.cpp
        framework/application.h
        framework/scene.h
        framework/mesh.h)
target_include_directories(framework PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(ProjectLaboratory src/main.cpp
        src/test/testmesh.h)

target_link_libraries(framework PUBLIC glfw glad OpenGL::GL glm)

target_link_libraries(ProjectLaboratory PRIVATE framework)
