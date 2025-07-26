# 创建静态库
set(LIB_SOURCES 
    mesh.cc 
    Renderer.cpp 
    Shader.cpp 
    Texture.cc 
    Camera.cpp
)

# 创建静态库
add_library(opengl_utils STATIC ${LIB_SOURCES})

# 设置库的属性
set_target_properties(opengl_utils PROPERTIES
    VERSION 1.0.0
    SOVERSION 1
)

# 链接依赖库
target_link_libraries(opengl_utils PUBLIC 
    glfw 
    glad::glad 
    glm::glm-header-only
)

# 设置包含目录
target_include_directories(opengl_utils PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# 导出库供其他目录使用
set(OPENGL_UTILS_LIB opengl_utils PARENT_SCOPE)

# 添加imgui子目录
# add_subdirectory(imgui)