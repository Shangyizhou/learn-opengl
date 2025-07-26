add_executable(hello_world hello_world.cc)
target_link_libraries(hello_world PRIVATE glfw glad::glad)
# 设置输出目录
set_target_properties(hello_world PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/learn/
) 