# 原有的example_factory程序
set(SRC example.cc mesh.cc Renderer.cpp Shader.cpp Texture.cc Camera.cpp MeshFactory.cc)
add_executable(mesh ${SRC})
target_link_libraries(mesh PRIVATE glfw glad::glad glm::glm-header-only)

# 新的mesh展示程序
set(SHOWCASE_SRC mesh_showcase.cc mesh.cc Renderer.cpp Shader.cpp Texture.cc Camera.cpp MeshFactory.cc)
add_executable(mesh_showcase ${SHOWCASE_SRC})
target_link_libraries(mesh_showcase PRIVATE glfw glad::glad glm::glm-header-only)

# 简单测试程序
set(SIMPLE_SRC simple_test.cc mesh.cc Renderer.cpp Shader.cpp Texture.cc)
add_executable(simple_test ${SIMPLE_SRC})
target_link_libraries(simple_test PRIVATE glfw glad::glad glm::glm-header-only)