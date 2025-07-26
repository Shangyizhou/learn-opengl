# 创建mesh示例程序
add_executable(mesh_example main.cc)
target_link_libraries(mesh_example PRIVATE opengl_utils)

# 设置输出目录
set_target_properties(mesh_example PROPERTIES
   RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/mesh/
) 