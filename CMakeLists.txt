cmake_minimum_required(VERSION 3.22)
project(ProjectLaboratory LANGUAGES C CXX)

# =========================================
# Framework
# =========================================

set(GLSL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/code/glsl/")
set(RESOURCES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/resources/")
add_subdirectory(framework)

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
set(ASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT OFF CACHE BOOL "" FORCE)

set(ASSIMP_BUILD_OBJ_IMPORTER ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_FBX_IMPORTER ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_GLTF_IMPORTER ON CACHE BOOL "" FORCE)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/libs/assimp")

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
    target_compile_options(ProjectLaboratory PRIVATE
            $<$<CONFIG:Release>:-march=native>
    )
endif()