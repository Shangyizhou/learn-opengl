#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>
#include "shader_proxy.h"
#include "buffer_manager.h"

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

GLFWwindow* initWindows() 
{
    glfwInit(); // 初始化 GLFW 库
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // 创建的 OpenGL 上下文的主要版本号为 3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // 创建的 OpenGL 上下文的次要版本号为 3

    /**
     * 这行代码指定了要使用的 OpenGL 配置文件。
     * 在此情况下，设置为 GLFW_OPENGL_CORE_PROFILE 表示希望使用核心模式的 OpenGL。
     * 核心模式不包含旧版功能（如固定功能管线），允许使用更现代的 OpenGL 编程方式（例如着色器和 VAO/VBO）。
    */
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
    }

   
    glfwMakeContextCurrent(window);  // 将指定的窗口的 OpenGL 上下文设为当前上下文，使得后续的 OpenGL 调用都将针对这个上下文。
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); // 注册窗口大小变化时的回调函数。在 OpenGL 应用程序中，窗口的大小变化时，你需要更新视口（viewport）以确保渲染内容能够正确显示。

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return nullptr;
    }

    return window;
}


int main()
{
    GLFWwindow* window = initWindows();
    ShaderProxy shader_proxy("./glsl/shader.vs", "./glsl/shader.fs");
    shader_proxy.compile();
    
    float vertices[] = {
        // positions         // colors
         0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  // bottom left
         0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f   // top 
    };
    BufferManager bufferManager(vertices, sizeof(vertices));
    // unsigned int VBO, VAO;
    // glGenVertexArrays(1, &VAO);
    // glGenBuffers(1, &VBO);
    // // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    // glBindVertexArray(VAO);

    // glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // // position attribute
    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    // glEnableVertexAttribArray(0);
    // // color attribute
    // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    // glEnableVertexAttribArray(1);

    // 检查窗口是否应该关闭。返回 true 表示窗口已请求关闭（如用户点击关闭按钮），循环会终止
    while (!glfwWindowShouldClose(window))
    {

        processInput(window);
      
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shader_proxy.use();
        glBindVertexArray(bufferManager.getVAO());
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // 交换缓冲区：glfwSwapBuffers(window); 将前缓冲区和后缓冲区交换，显示新绘制的帧。这是双缓冲技术的一部分，可以减少撕裂现象。
        glfwSwapBuffers(window);
        // 处理事件：glfwPollEvents(); 处理所有待处理的事件，如键盘输入、鼠标移动等，确保应用程序能够响应用户操作。
        glfwPollEvents();
    }

    bufferManager.release();
    glfwTerminate();

    return 0;
}

void processInput(GLFWwindow* window)
{
    // 处理用户的输入 主要检查是否按下了 ESC 键
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true); // 按下 ESC 则退出
}

/**
 * 这个回调函数的作用是当窗口尺寸发生变化时（例如用户调整窗口大小），更新 OpenGL 的视口设置，使得渲染输出适应新的窗口大小。
 * 这样可以确保渲染的内容正确显示在窗口中，而不会出现失真或裁剪的情况。
*/
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
