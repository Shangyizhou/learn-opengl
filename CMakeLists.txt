cmake_minimum_required(VERSION 3.5)

set(CMAKE_TOOLCHAIN_FILE "/home/shangyizhou/code/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")
list(APPEND CMAKE_PREFIX_PATH "/home/shangyizhou/code/cpp/vcpkg/installed/x64-linux")

project(learn-opengl-demo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(glfw3 REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(Stb REQUIRED)
find_package(glm CONFIG REQUIRED)

# Set compile-time flags (same as your CXXFLAGS)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -g")

# Define output directory (same as OUTPUT in your Makefile)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# Add the include directories
# include_directories(${PROJECT_SOURCE_DIR}/include)

# Add the executable
add_subdirectory(src/01_Hello_Window)
add_subdirectory(src/02_Hello_Triangle)
add_subdirectory(src/03_Shaders)
add_subdirectory(src/04_Textures)
add_subdirectory(src/05_Transformations)
add_subdirectory(src/06_Coordinate_Systems)
# add_subdirectory(src/07_Camera)
add_subdirectory(src/pratice)