set(SRC example_01.cc)
add_executable(example_01 ${SRC})
target_link_libraries(example_01 PRIVATE glfw glad::glad glm::glm-header-only imgui::imgui)