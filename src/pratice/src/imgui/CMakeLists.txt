add_executable(example_01 example_01.cc)
target_link_libraries(example_01 PRIVATE glfw glad::glad glm::glm-header-only imgui::imgui)

add_executable(example_02 example_02.cc)
target_link_libraries(example_02 PRIVATE glfw glad::glad glm::glm-header-only imgui::imgui)

add_executable(camera_control_demo camera_control_demo.cc ../mesh.cc ../Renderer.cpp ../Shader.cpp ../Texture.cc ../Camera.cpp)
target_link_libraries(camera_control_demo PRIVATE glfw glad::glad glm::glm-header-only imgui::imgui)

add_executable(enhanced_camera_demo enhanced_camera_demo.cc ../mesh.cc ../Renderer.cpp ../Shader.cpp ../Texture.cc ../Camera.cpp)
target_link_libraries(enhanced_camera_demo PRIVATE glfw glad::glad glm::glm-header-only imgui::imgui)
