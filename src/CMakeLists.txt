set(APP_NAME Water)

include_directories(Buffer)
include_directories(Camera)
include_directories(CString)
include_directories(Mathematics)
include_directories(OpenGLView)
include_directories(OpenGLRenderer)
include_directories(ShaderProgram)
include_directories(Texture)

############################################################################################

set(BUFFER_FILES
    Buffer/buffer.cpp
    Buffer/buffer.hpp)

set(CAMERA_FILES
    Camera/camera.cpp
    Camera/camera.hpp)

set(CSTRING_FILES
    CString/cstring.cpp
    CString/cstring.hpp)

set(MATHEMATICS_FILES
    Mathematics/mathematics.cpp
    Mathematics/mathematics.hpp)

set(OPENGL_RENDERER_FILES
    OpenGLRenderer/opengl_renderer.cpp
    OpenGLRenderer/opengl_renderer.hpp)

set(OPENGL_VIEW_FILES
    OpenGLView/opengl_view.cpp
    OpenGLView/opengl_view.hpp)

set(SHADER_PROGRAM_FILES
    ShaderProgram/shader_program.cpp
    ShaderProgram/shader_program.hpp)

set(SHADERS_FILES
    ${PROJECT_SOURCE_DIR}/bin/cube_map_blur.vs
    ${PROJECT_SOURCE_DIR}/bin/cube_map_hblur.fs
    ${PROJECT_SOURCE_DIR}/bin/cube_map_vblur.fs
    ${PROJECT_SOURCE_DIR}/bin/photon.fs
    ${PROJECT_SOURCE_DIR}/bin/photon.vs
    ${PROJECT_SOURCE_DIR}/bin/poolsky.fs
    ${PROJECT_SOURCE_DIR}/bin/poolsky.vs
    ${PROJECT_SOURCE_DIR}/bin/water.fs
    ${PROJECT_SOURCE_DIR}/bin/water.vs
    ${PROJECT_SOURCE_DIR}/bin/wateradddrop.fs
    ${PROJECT_SOURCE_DIR}/bin/wateradddrop.vs
    ${PROJECT_SOURCE_DIR}/bin/waterheightmap.fs
    ${PROJECT_SOURCE_DIR}/bin/waterheightmap.vs
    ${PROJECT_SOURCE_DIR}/bin/waternormalmap.fs
    ${PROJECT_SOURCE_DIR}/bin/waternormalmap.vs)

set(TEXTURE_FILES
    Texture/texture.cpp
    Texture/texture.hpp)

set(HOST_FILES
    ${BUFFER_FILES}
    ${CAMERA_FILES}
    ${CSTRING_FILES}
    ${MATHEMATICS_FILES}
    ${OPENGL_RENDERER_FILES}
    ${OPENGL_VIEW_FILES}
    ${SHADER_PROGRAM_FILES}
    ${SHADERS_FILES}
    ${TEXTURE_FILES}
    main.cpp)


set(CUDA_FILES hello.cu)

############################################################################################

cuda_compile(CUDA_ALL_FILES ${CUDA_FILES})

cuda_add_executable(${APP_NAME} WIN32 ${CUDA_ALL_FILES} ${HOST_FILES})
target_link_libraries(${APP_NAME} ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} ${GLEW_LIBRARIES} ${FREEIMAGE_LIBRARY})

set_target_properties(${APP_NAME} PROPERTIES OUTPUT_NAME_DEBUG "${APP_NAME}d")
set_target_properties(${APP_NAME} PROPERTIES OUTPUT_NAME_RELEASE ${APP_NAME})

set_target_properties(${APP_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set_target_properties(${APP_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/bin)
set_target_properties(${APP_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/bin)


############################################################################################

if (MSVC)
    source_group("Source Files\\" FILES main.cpp)

    source_group("Source Files\\Buffer" FILES ${BUFFER_FILES})
    source_group("Source Files\\Camera" FILES ${CAMERA_FILES})
    source_group("Source Files\\CString" FILES ${CSTRING_FILES})
    source_group("Source Files\\Mathematics" FILES ${MATHEMATICS_FILES})
    source_group("Source Files\\OpenGLRenderer" FILES ${OPENGL_RENDERER_FILES})
    source_group("Source Files\\OpenGLView" FILES ${OPENGL_VIEW_FILES})
    source_group("Source Files\\ShaderProgram" FILES ${SHADER_PROGRAM_FILES})
    source_group("Source Files\\Texture" FILES ${TEXTURE_FILES})

    source_group("CUDA" FILES ${CUDA_FILES})
    source_group("Shaders" FILES ${SHADERS_FILES})

endif (MSVC)

############################################################################################


# Project settings
if (MSVC)
    set(CMAKE_CXX_FLAGS "/EHsc /W3 /nologo" CACHE STRING "" FORCE)

    set(CMAKE_CXX_FLAGS_DEBUG "/D_DEBUG /MDd /Zi /Ob0 /Od /RTC1")
    set(CMAKE_CXX_FLAGS_MINSIZEREL     "/MT /O1 /Ob1 /D NDEBUG")
    set(CMAKE_CXX_FLAGS_RELEASE        "/MT /O2 /Ob2 /D NDEBUG")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "/MT /Zi /O2 /Ob1 /D NDEBUG")

    set_target_properties(${APP_NAME} PROPERTIES LINK_FLAGS " /SAFESEH:NO /DYNAMICBASE:NO /FIXED")

elseif (CMAKE_COMPILER_IS_GNUCXX)
    # Setting directory
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIRECTORY} CACHE STRING "" FORCE)

    # Compiler flags
    set(CMAKE_CXX_FLAGS " -std=c++11 -Wall " CACHE STRING "" FORCE)
    set(CMAKE_CXX_FLAGS_DEBUG " -g -Os " CACHE STRING "" FORCE)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}" CACHE STRING "" FORCE)

    # Linker flags
    set(CMAKE_EXE_LINKER_FLAGS " -static-libgcc -static-libstdc++ -static " CACHE STRING "" FORCE)
    set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "" CACHE STRING "" FORCE) # remove dynamic for CXX
    set(BUILD_SHARED_LIBS OFF)
endif (MSVC)

############################################################################################
