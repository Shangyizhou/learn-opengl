# 原有的example_factory程序
set(SRC mesh.cc Renderer.cpp Shader.cpp Texture.cc Camera.cpp)

add_executable(mesh main.cc ${SRC})
target_link_libraries(mesh PRIVATE glfw glad::glad glm::glm-header-only)

# 添加光照demo
add_executable(lighting_demo lighting_demo.cc ${SRC})
target_link_libraries(lighting_demo PRIVATE glfw glad::glad glm::glm-header-only)

# 添加Phong光照demo
add_executable(phong_lighting_demo phong_lighting_demo.cc ${SRC})
target_link_libraries(phong_lighting_demo PRIVATE glfw glad::glad glm::glm-header-only)

# 添加光照贴图demo
add_executable(lighting_maps_demo lighting_maps_demo.cc ${SRC})
target_link_libraries(lighting_maps_demo PRIVATE glfw glad::glad glm::glm-header-only)

# 添加投光物demo
add_executable(light_casters_demo light_casters_demo.cc ${SRC})
target_link_libraries(light_casters_demo PRIVATE glfw glad::glad glm::glm-header-only)

# 添加多光源demo
add_executable(multiple_lights_demo multiple_lights_demo.cc ${SRC})
target_link_libraries(multiple_lights_demo PRIVATE glfw glad::glad glm::glm-header-only)

add_subdirectory(imgui)