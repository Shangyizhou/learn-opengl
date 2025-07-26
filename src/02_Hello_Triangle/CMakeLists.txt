add_executable(triangle triangle.cc)
target_link_libraries(triangle PRIVATE glfw glad::glad)

set_target_properties(triangle PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/learn/
)