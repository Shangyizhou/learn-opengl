# 原有的example_factory程序
set(SRC main.cc mesh.cc Renderer.cpp Shader.cpp Texture.cc Camera.cpp)
add_executable(mesh ${SRC})
target_link_libraries(mesh PRIVATE glfw glad::glad glm::glm-header-only)

add_subdirectory(imgui)