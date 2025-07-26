add_executable(transformations transformations.cc)
target_link_libraries(transformations PRIVATE glfw glad::glad glm::glm-header-only)

set_target_properties(transformations PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/learn/
)