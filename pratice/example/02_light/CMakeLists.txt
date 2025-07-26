# 创建光照相关的示例程序

# 1. 基础光照示例
add_executable(lighting_example lighting_demo.cc)
target_link_libraries(lighting_example PRIVATE opengl_utils)

# 2. Phong光照示例
add_executable(phong_lighting_example phong_lighting_demo.cc)
target_link_libraries(phong_lighting_example PRIVATE opengl_utils)

# 3. 光照贴图示例
add_executable(lighting_maps_example lighting_maps_demo.cc)
target_link_libraries(lighting_maps_example PRIVATE opengl_utils)

# 4. 投光物示例
add_executable(light_casters_example light_casters_demo.cc)
target_link_libraries(light_casters_example PRIVATE opengl_utils)

# 5. 多光源示例
add_executable(multiple_lights_example multiple_lights_demo.cc)
target_link_libraries(multiple_lights_example PRIVATE opengl_utils)

# 设置所有光照示例程序的输出目录
set_target_properties(
    lighting_example
    phong_lighting_example
    lighting_maps_example
    light_casters_example
    multiple_lights_example
    PROPERTIES
   RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/light/
) 