cmake_minimum_required (VERSION 3.12)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("Wasteland")

file(GLOB_RECURSE SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/Wasteland/Source/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/Library/Source/*cpp")
file(GLOB_RECURSE HEADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/Wasteland/Header/*.hpp")

add_executable(Wasteland ${SOURCE_FILES} ${HEADER_FILES})

target_include_directories(Wasteland PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Wasteland/Header")
include_directories(Wasteland PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Library/Header")

message(STATUS "Include directories: ${CMAKE_CURRENT_SOURCE_DIR}/Wasteland/Header")

include(FetchContent)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG latest
)

FetchContent_MakeAvailable(glfw)

if(LINUX)
  find_package(OpenGL REQUIRED)
  target_link_libraries(Wasteland PRIVATE glfw OpenGL::GL)
  add_definitions(-D_GLFW_X11)
elseif(WIN32)
  target_link_libraries(Wasteland PRIVATE glfw opengl32)
endif()

FetchContent_Declare(
    FreeImage
    GIT_REPOSITORY https://github.com/danoli3/FreeImage.git
    GIT_TAG master
)

FetchContent_MakeAvailable(FreeImage)

target_include_directories(Wasteland PRIVATE "${FreeImage_SOURCE_DIR}/Source")

target_compile_definitions(Wasteland PRIVATE FREEIMAGE_LIB)

target_link_libraries(Wasteland PRIVATE FreeImage)

set(JUST_INSTALL_CEREAL ON CACHE BOOL "Skip building tests/examples and dependency checks." FORCE)

FetchContent_Declare(
    Cereal
    GIT_REPOSITORY https://github.com/USCiLab/cereal.git
    GIT_TAG master
)

FetchContent_MakeAvailable(Cereal)

target_link_libraries(Wasteland PRIVATE cereal)

set(SOURCE_ASSETS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Assets")
set(BUILD_ASSETS_DIR "${CMAKE_CURRENT_BINARY_DIR}/Assets")

add_custom_command(
    TARGET Wasteland POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${SOURCE_ASSETS_DIR} ${BUILD_ASSETS_DIR}
    COMMENT "Copying Assets directory to build folder..."
)

if(APPLE)
  message(STATUS "Building on macOS: Including necessary frameworks and SDKs")

  target_link_libraries(Wasteland PRIVATE
  glfw
  GL
  "-framework Cocoa"
  "-framework QuartzCore"
  "-framework OpenGL"
  "-framework IOKit"
  "-framework CoreVideo"
  "-framework CoreFoundation"
  "-framework Metal"
  "-framework MetalKit"
  "-framework AVFoundation"
  )

  target_compile_options(Wasteland PRIVATE
    -Wall
    -Wextra
    -Wno-deprecated-declarations
    -mmacosx-version-min=13.7.4
  )

  include_directories(
    /System/Library/Frameworks
    /Library/Frameworks
  )
endif()