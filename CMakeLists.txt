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

add_executable(ProjectLaboratory src/main.cpp)

target_link_libraries(ProjectLaboratory glfw glad OpenGL::GL glm)
