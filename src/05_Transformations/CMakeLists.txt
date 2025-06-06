add_executable(transformations transformations.cc)
target_link_libraries(transformations PRIVATE glfw glad::glad)
 target_link_libraries(transformations PRIVATE glm::glm-header-only)