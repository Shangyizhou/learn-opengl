set(SRC example.cc mesh.cc Renderer.cpp Shader.cpp Texture.cc Camera.cpp)
add_executable(mesh ${SRC})
target_link_libraries(mesh PRIVATE glfw glad::glad glm::glm-header-only)