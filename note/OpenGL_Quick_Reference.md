# OpenGL 快速参考手册

## 核心概念速查

### 1. 渲染管线流程
```
顶点数据 → 顶点着色器 → 图元装配 → 光栅化 → 片段着色器 → 测试与混合 → 帧缓冲
```

### 2. MVP矩阵变换
```cpp
// 模型矩阵 (Model) - 局部到世界
model = glm::translate(model, position);
model = glm::rotate(model, angle, axis);
model = glm::scale(model, scale);

// 视图矩阵 (View) - 世界到相机
view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

// 投影矩阵 (Projection) - 3D到2D
projection = glm::perspective(glm::radians(fov), aspect, near, far);

// 最终变换
gl_Position = projection * view * model * vec4(position, 1.0);
```

### 3. 缓冲区对象
```cpp
// VAO - 顶点数组对象
glGenVertexArrays(1, &VAO);
glBindVertexArray(VAO);

// VBO - 顶点缓冲对象
glGenBuffers(1, &VBO);
glBindBuffer(GL_ARRAY_BUFFER, VBO);
glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);

// EBO - 元素缓冲对象
glGenBuffers(1, &EBO);
glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW);
```

### 4. 顶点属性设置
```cpp
// 位置属性
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
glEnableVertexAttribArray(0);

// 颜色属性
glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
glEnableVertexAttribArray(1);
```

### 5. 着色器基础
```glsl
// 顶点着色器
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 ourColor;
uniform mat4 transform;

void main() {
    gl_Position = transform * vec4(aPos, 1.0);
    ourColor = aColor;
}

// 片段着色器
#version 330 core
in vec3 ourColor;
out vec4 FragColor;

void main() {
    FragColor = vec4(ourColor, 1.0);
}
```

### 6. 相机控制
```cpp
// FPS相机
class Camera {
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    float yaw, pitch;
    
    void processKeyboard(GLFWwindow* window, float deltaTime) {
        float velocity = speed * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            position += front * velocity;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            position -= front * velocity;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            position -= glm::normalize(glm::cross(front, up)) * velocity;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            position += glm::normalize(glm::cross(front, up)) * velocity;
    }
    
    void processMouseMovement(float xoffset, float yoffset) {
        yaw += xoffset * sensitivity;
        pitch += yoffset * sensitivity;
        
        if (pitch > 89.0f) pitch = 89.0f;
        if (pitch < -89.0f) pitch = -89.0f;
        
        updateCameraVectors();
    }
};
```

### 7. 绘制调用
```cpp
// 三角形绘制
glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

// 线框绘制
glDrawElements(GL_LINES, 6, GL_UNSIGNED_INT, 0);

// 点绘制
glDrawElements(GL_POINTS, 6, GL_UNSIGNED_INT, 0);
```

### 8. 状态设置
```cpp
// 深度测试
glEnable(GL_DEPTH_TEST);
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

// 混合
glEnable(GL_BLEND);
glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

// 线框模式
glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

// 背面剔除
glEnable(GL_CULL_FACE);
glCullFace(GL_BACK);
```

### 9. 时间管理
```cpp
float currentFrame = glfwGetTime();
deltaTime = currentFrame - lastFrame;
lastFrame = currentFrame;

// 帧率无关的移动
position += velocity * deltaTime;
```

### 10. 错误检查
```cpp
void checkGLErrors() {
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        std::cout << "OpenGL error: " << err << std::endl;
    }
}
```

## 常用函数速查

### 初始化
- `glfwInit()` - 初始化GLFW
- `glfwCreateWindow()` - 创建窗口
- `glfwMakeContextCurrent()` - 设置当前上下文
- `gladLoadGLLoader()` - 加载OpenGL函数

### 缓冲区操作
- `glGenBuffers()` - 生成缓冲区
- `glBindBuffer()` - 绑定缓冲区
- `glBufferData()` - 上传数据
- `glVertexAttribPointer()` - 设置顶点属性
- `glEnableVertexAttribArray()` - 启用顶点属性

### 着色器操作
- `glCreateShader()` - 创建着色器
- `glShaderSource()` - 设置着色器源码
- `glCompileShader()` - 编译着色器
- `glCreateProgram()` - 创建程序
- `glAttachShader()` - 附加着色器
- `glLinkProgram()` - 链接着色器程序
- `glUseProgram()` - 使用着色器程序

### 绘制操作
- `glClear()` - 清除缓冲区
- `glDrawArrays()` - 绘制数组
- `glDrawElements()` - 绘制索引
- `glfwSwapBuffers()` - 交换缓冲区
- `glfwPollEvents()` - 处理事件

### 矩阵操作 (GLM)
- `glm::translate()` - 平移矩阵
- `glm::rotate()` - 旋转矩阵
- `glm::scale()` - 缩放矩阵
- `glm::lookAt()` - 视图矩阵
- `glm::perspective()` - 透视投影
- `glm::ortho()` - 正交投影

## 常见问题解决

### 黑屏问题
1. 检查着色器编译是否成功
2. 检查矩阵变换顺序
3. 检查顶点数据是否正确
4. 检查OpenGL版本兼容性

### 性能问题
1. 减少绘制调用次数
2. 使用索引绘制
3. 合理使用VBO和VAO
4. 避免每帧重新编译着色器

### 坐标问题
1. 检查MVP矩阵顺序
2. 检查坐标系定义
3. 检查投影参数
4. 检查相机位置和方向

## 调试技巧

### 可视化调试
```cpp
// 显示线框
glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

// 显示法线
// 在片段着色器中输出法线作为颜色

// 显示深度
// 在片段着色器中输出深度值作为颜色
```

### 信息输出
```cpp
// 输出矩阵信息
std::cout << "Model Matrix:" << glm::to_string(model) << std::endl;

// 输出相机信息
std::cout << "Camera Position: " << glm::to_string(camera.position) << std::endl;

// 输出性能信息
std::cout << "FPS: " << 1.0f / deltaTime << std::endl;
```

这个快速参考手册涵盖了OpenGL开发中最常用的概念和函数，可以作为日常开发的快速查阅工具。 