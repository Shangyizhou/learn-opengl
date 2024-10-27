cmake_minimum_required(VERSION 3.5)
project(learn-opengl-demo)

project(MainProject LANGUAGES CXX)

set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_C_COMPILER "C:/MinGW/bin/gcc.exe")
set(CMAKE_CXX_COMPILER "C:/MinGW/bin/g++.exe")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set compile-time flags (same as your CXXFLAGS)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -g")

# Define output directory (same as OUTPUT in your Makefile)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# Add the include directories
include_directories(${PROJECT_SOURCE_DIR}/include)

# Add the executable
add_subdirectory(src/01)