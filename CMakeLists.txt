cmake_minimum_required(VERSION 3.2)
project(3DViewer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(TDVIEWER_DEBUG)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_definitions(TDVIEWER_RELEASE)
elseif(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    add_compile_definitions(TDVIEWER_RELWITHDEBINFO)
elseif(CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
    add_compile_definitions(TDVIEWER_MINSIZEREL)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin-int/${CMAKE_BUILD_TYPE})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin-int/${CMAKE_BUILD_TYPE})

if(MSVC)
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8")
   set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /utf-8")
endif()


# GLFW
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Build the GLFW example programs")
set(GLFW_BUILD_TESTS OFF CACHE BOOL "Build the GLFW test programs")
set(GLFW_BUILD_DOCS OFF CACHE BOOL "Build the GLFW documentation")
set(GLFW_INSTALL OFF CACHE BOOL "Generate installation target")
set(GLFW_USE_WAYLAND ON CACHE BOOL "Enable Wayland support")

set(GLFW_DIR "${CMAKE_SOURCE_DIR}/dependencies/glfw")
add_subdirectory(${GLFW_DIR})
include_directories(SYSTEM ${GLFW_DIR}/include)
##

# GLM
include_directories(SYSTEM dependencies/glm/)
##

# stb_image
include_directories(SYSTEM dependencies/stb_image/src)
##

# GLAD
include_directories(SYSTEM dependencies/glad/src)
include_directories(SYSTEM dependencies/glad/include)
add_library(glad dependencies/glad/src/glad.c)
##

# imgui
include_directories(SYSTEM dependencies/imgui)
file(GLOB IMGUI_SOURCES "dependencies/imgui/*.cpp"
"dependencies/imgui/backends/imgui_impl_glfw.cpp"
"dependencies/imgui/backends/imgui_impl_opengl3.cpp")
add_library(imgui ${IMGUI_SOURCES})
##

# ImGuizmo
include_directories(SYSTEM dependencies/ImGuizmo)
file(GLOB IMGUIZMO_SOURCES "dependencies/ImGuizmo/*.cpp")
add_library(imguizmo ${IMGUIZMO_SOURCES})
##

# Assimp
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "Disable building tests")
set(ASSIMP_INSTALL OFF CACHE BOOL "Disable lib installing")
set(ASSIMP_WARNINGS_AS_ERRORS OFF CACHE BOOL "Disable treating warning as errors")

set(ASSIMP_DIR "${CMAKE_SOURCE_DIR}/dependencies/assimp")
include_directories(SYSTEM ${ASSIMP_DIR}/include)
add_subdirectory(${ASSIMP_DIR})
##

# NativeFileDialog
set(NFD_INSTALL OFF CACHE BOOL "Disable lib installing")
set(NFD_PORTAL ON CACHE BOOL "Enable linux portal")
include_directories(SYSTEM ${CMAKE_SOURCE_DIR}/dependencies/nativefiledialog-extended/src/include)
add_subdirectory(${CMAKE_SOURCE_DIR}/dependencies/nativefiledialog-extended/)
set_target_properties(assimp PROPERTIES COMPILE_FLAGS "-w")
##

file(GLOB_RECURSE SOURCES "src/*.hpp" "src/*.cpp" "src/*.h")

add_executable(3DViewer ${SOURCES})
target_compile_options(3DViewer PRIVATE -Wall -Wno-narrowing -Wextra -Wno-deprecated-copy-with-user-provided-dtor)
if(WIN32) # Pokud je operační systém Windows
    set(CMAKE_C_COMPILER "cl")
    set(CMAKE_CXX_COMPILER "cl")
    set_target_properties(3DViewer PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
    target_link_libraries(3DViewer glad imgui imguizmo glfw assimp nfd)
elseif(UNIX AND NOT APPLE) # Pokud je operační systém Linux
    set(CMAKE_C_COMPILER "clang")
    set(CMAKE_CXX_COMPILER "clang++")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsized-deallocation")

    if(NOT DEFINED CMAKE_BUILD_TYPE)
		set(CMAKE_BUILD_TYPE Release)
    endif()
    target_link_libraries(3DViewer PUBLIC glad glfw imgui imguizmo assimp pthread nfd)
endif()

