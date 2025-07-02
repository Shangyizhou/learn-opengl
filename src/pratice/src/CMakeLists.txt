set(SRC example.cc mesh.cc mesh.cc Renderer.cpp Shader.cpp)
add_executable(mesh ${SRC})
target_link_libraries(mesh PRIVATE glfw glad::glad glm::glm-header-only)