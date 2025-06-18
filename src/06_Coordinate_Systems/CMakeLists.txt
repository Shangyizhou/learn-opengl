add_executable(coordinate_systems coordinate_systems.cc)
target_link_libraries(coordinate_systems PRIVATE glfw glad::glad glm::glm-header-only)