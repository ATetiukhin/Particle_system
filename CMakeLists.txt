cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)

# Project settings
set(PROJECT_NAME Particle_system)
project(${PROJECT_NAME})

set(VERSION_MAJOR 0)
set(VERSION_MINOR 0)
set(VERSION_PATCH 1)
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

# CUDA
find_package(CUDA REQUIRED)
if (CUDA_FOUND)
  set(CUDA_NVCC_FLAGS -gencode arch=compute_30,code=sm_30 CACHE STRING "" FORCE)
  set(CUDA_NVCC_FLAGS_DEBUG -G CACHE STRING "" FORCE)
endif (CUDA_FOUND)

############################################################################################

add_subdirectory(src)

############################################################################################
