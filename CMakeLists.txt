# CMakeLists.txt

cmake_minimum_required(VERSION 3.21)

project(lab25)

# CMake variables
get_filename_component(ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR} ABSOLUTE)

# CMake compilation flags
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20 CACHE INTERNAL "This is a read-only variable")

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE
        Debug
        CACHE STRING
        "Choose the type of build, options are: Debug, Release, RelWithDebInfo, MinSizeRel."
        FORCE
    )
endif()

# Link libtorch
if(NOT Torch_FOUND)
    find_package(Torch REQUIRED)
endif(NOT Torch_FOUND)

# CMake options
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Subdirectories
add_subdirectory(demo)
add_subdirectory(emulator)
add_subdirectory(third-party)
