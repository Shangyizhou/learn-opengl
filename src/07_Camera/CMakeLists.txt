add_executable(camera camera.cc)
target_link_libraries(camera PRIVATE glfw glad::glad glm::glm-header-only)