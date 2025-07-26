add_executable(coordinate_systems coordinate_systems.cc)
target_link_libraries(coordinate_systems PRIVATE glfw glad::glad glm::glm-header-only)

set_target_properties(coordinate_systems PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/learn/
)