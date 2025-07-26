add_executable(shaders shaders.cc)
target_link_libraries(shaders PRIVATE glfw glad::glad)

set_target_properties(shaders PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/learn/
)