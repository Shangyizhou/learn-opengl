add_executable(transformations transformations.cc)
target_link_libraries(transformations PRIVATE glfw glad::glad glm::glm-header-only)