add_executable(camera camera.cc)
target_link_libraries(camera PRIVATE glfw glad::glad glm::glm-header-only)

set_target_properties(camera PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/learn/
)