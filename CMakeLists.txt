cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)

# Project settings
set(PROJECT_NAME Particle_system)
project(${PROJECT_NAME})

set(VERSION_MAJOR 0)
set(VERSION_MINOR 0)
set(VERSION_PATCH 2)
set(VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH})
message(STATUS "VERSION: " ${VERSION})

############################################################################################

# OS settings
if (WIN32 OR APPLE)
    set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR}/build/install CACHE STRING "" FORCE)
    set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)
endif(WIN32 OR APPLE)

# Cmake settings
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake)

############################################################################################

# OpenGL
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})

# Glut
set(CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/libs/glut/")
set(CMAKE_LIBRARY_PATH "${CMAKE_CURRENT_SOURCE_DIR}/libs/glut/" CACHE FILEPATH "Root directory of Glew installation")

# set(GLUT_ROOT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/libs/glut/" CACHE FILEPATH "Root directory of Glut installation") 
# set(GLUT_glut_LIBRARY "${GLUT_ROOT_PATH}/lib/glut32.lib" CACHE FILEPATH "glut library") 

find_package(GLUT REQUIRED)
include_directories(${GLUT_INCLUDE_DIR})

# Glew
set(CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/libs/glew/")
set(CMAKE_LIBRARY_PATH "${CMAKE_CURRENT_SOURCE_DIR}/libs/glew/" CACHE FILEPATH "Root directory of Glew installation")

find_package(GLEW REQUIRED)
if (GLEW_FOUND)
    include_directories(${GLEW_INCLUDE_DIRS})
    link_libraries(${GLEW_LIBRARIES})
endif()

############################################################################################

# CUDA
find_package(CUDA REQUIRED)
if (CUDA_FOUND)
  set(CUDA_NVCC_FLAGS -gencode arch=compute_30,code=sm_30 CACHE STRING "" FORCE)
  set(CUDA_NVCC_FLAGS_DEBUG -G CACHE STRING "" FORCE)
endif (CUDA_FOUND)

############################################################################################

# FreeImage

set(FREEIMAGE_LIBRARY_AND_HEADER_DIRRECTORY libs/FreeImage)
find_library(FREEIMAGE_LIBRARY
NAMES
    freeimage
PATHS
    ${FREEIMAGE_LIBRARY_AND_HEADER_DIRRECTORY}
)
find_path(FREEIMAGE_LIBRARY_AND_HEADER_DIRRECTORY FreeImage.h)
include_directories(${FREEIMAGE_LIBRARY_AND_HEADER_DIRRECTORY})

############################################################################################

add_subdirectory(src)

############################################################################################
