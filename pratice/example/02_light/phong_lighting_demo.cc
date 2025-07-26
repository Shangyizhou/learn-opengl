#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#include "Shader.h"
#include "Camera.h"
#include "mesh.h"

// 窗口设置
const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 800;

// 相机
Camera camera(glm::vec3(0.0f, 1.0f, 6.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// 时间
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// 光照参数
glm::vec3 lightPos(2.0f, 3.0f, 2.0f);
bool lightMoving = true;
float lightSpeed = 2.0f;

// 材质参数
float ambientStrength = 0.1f;
float diffuseStrength = 1.0f;
float specularStrength = 0.5f;
float shininess = 32.0f;

// 颜色参数
glm::vec3 objectColor(1.0f, 0.5f, 0.31f);  // 珊瑚红色
glm::vec3 lightColor(1.0f, 1.0f, 1.0f);    // 白色光源

// 回调函数
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

int main()
{
    // 初始化GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // 创建窗口
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Phong Lighting Demo", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);

    // 初始化GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // 启用深度测试
    glEnable(GL_DEPTH_TEST);

    // 创建Phong光照着色器
    Shader phongShader(
        // 顶点着色器
        "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "layout (location = 1) in vec3 aColor;\n"
        "layout (location = 2) in vec3 aNormal;\n"
        "layout (location = 3) in vec2 aTexCoord;\n"
        "\n"
        "out vec3 FragPos;\n"
        "out vec3 Normal;\n"
        "out vec3 Color;\n"
        "\n"
        "uniform mat4 model;\n"
        "uniform mat4 view;\n"
        "uniform mat4 projection;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    FragPos = vec3(model * vec4(aPos, 1.0));\n"
        "    Normal = mat3(transpose(inverse(model))) * aNormal;\n"
        "    Color = aColor;\n"
        "    gl_Position = projection * view * vec4(FragPos, 1.0);\n"
        "}\n",
        // 片段着色器 - 完整的Phong光照模型
        "#version 330 core\n"
        "out vec4 FragColor;\n"
        "\n"
        "in vec3 FragPos;\n"
        "in vec3 Normal;\n"
        "in vec3 Color;\n"
        "\n"
        "uniform vec3 lightPos;\n"
        "uniform vec3 viewPos;\n"
        "uniform vec3 lightColor;\n"
        "uniform vec3 objectColor;\n"
        "uniform float ambientStrength;\n"
        "uniform float diffuseStrength;\n"
        "uniform float specularStrength;\n"
        "uniform float shininess;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    // 环境光照\n"
        "    vec3 ambient = ambientStrength * lightColor;\n"
        "\n"
        "    // 漫反射光照\n"
        "    vec3 norm = normalize(Normal);\n"
        "    vec3 lightDir = normalize(lightPos - FragPos);\n"
        "    float diff = max(dot(norm, lightDir), 0.0);\n"
        "    vec3 diffuse = diffuseStrength * diff * lightColor;\n"
        "\n"
        "    // 镜面光照\n"
        "    vec3 viewDir = normalize(viewPos - FragPos);\n"
        "    vec3 reflectDir = reflect(-lightDir, norm);\n"
        "    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);\n"
        "    vec3 specular = specularStrength * spec * lightColor;\n"
        "\n"
        "    // 合并所有光照分量\n"
        "    vec3 result = (ambient + diffuse + specular) * objectColor;\n"
        "    FragColor = vec4(result, 1.0);\n"
        "}\n",
        true
    );

    // 光源着色器
    Shader lightShader(
        // 顶点着色器
        "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "layout (location = 1) in vec3 aColor;\n"
        "layout (location = 2) in vec3 aNormal;\n"
        "layout (location = 3) in vec2 aTexCoord;\n"
        "\n"
        "uniform mat4 model;\n"
        "uniform mat4 view;\n"
        "uniform mat4 projection;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
        "}\n",
        // 片段着色器
        "#version 330 core\n"
        "out vec4 FragColor;\n"
        "\n"
        "uniform vec3 lightColor;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    FragColor = vec4(lightColor, 1.0);\n"
        "}\n",
        true
    );

    // 创建立方体顶点数据
    std::vector<Vertex> vertices = {
        // 位置                // 颜色         // 法线         // 纹理坐标
        // 后面
        {{-0.5f, -0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}},
        {{ 0.5f, -0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}},
        {{ 0.5f,  0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}},
        {{ 0.5f,  0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}},
        {{-0.5f,  0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}},
        {{-0.5f, -0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}},

        // 前面
        {{-0.5f, -0.5f,  0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
        {{ 0.5f, -0.5f,  0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
        {{ 0.5f,  0.5f,  0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
        {{ 0.5f,  0.5f,  0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
        {{-0.5f,  0.5f,  0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
        {{-0.5f, -0.5f,  0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},

        // 左面
        {{-0.5f,  0.5f,  0.5f}, {1.0f, 1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
        {{-0.5f,  0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},
        {{-0.5f, -0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
        {{-0.5f, -0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
        {{-0.5f, -0.5f,  0.5f}, {1.0f, 1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
        {{-0.5f,  0.5f,  0.5f}, {1.0f, 1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},

        // 右面
        {{ 0.5f,  0.5f,  0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
        {{ 0.5f,  0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},
        {{ 0.5f, -0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
        {{ 0.5f, -0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
        {{ 0.5f, -0.5f,  0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
        {{ 0.5f,  0.5f,  0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},

        // 下面
        {{-0.5f, -0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f}},
        {{ 0.5f, -0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f}},
        {{ 0.5f, -0.5f,  0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f}},
        {{ 0.5f, -0.5f,  0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f}},
        {{-0.5f, -0.5f,  0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f}},
        {{-0.5f, -0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f}},

        // 上面
        {{-0.5f,  0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
        {{ 0.5f,  0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},
        {{ 0.5f,  0.5f,  0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
        {{ 0.5f,  0.5f,  0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
        {{-0.5f,  0.5f,  0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
        {{-0.5f,  0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}}
    };

    // 创建索引数据
    std::vector<uint32_t> indices;
    for (int i = 0; i < 36; i += 6) {
        indices.push_back(i);
        indices.push_back(i + 1);
        indices.push_back(i + 2);
        indices.push_back(i + 2);
        indices.push_back(i + 3);
        indices.push_back(i + 4);
    }

    // 创建网格
    Mesh cubeMesh(vertices, indices);
    
    std::cout << "=== Phong光照模型演示 ===" << std::endl;
    std::cout << "控制说明：" << std::endl;
    std::cout << "- WASD: 移动相机" << std::endl;
    std::cout << "- 鼠标: 旋转视角" << std::endl;
    std::cout << "- 滚轮: 缩放" << std::endl;
    std::cout << "- 空格: 切换光源移动" << std::endl;
    std::cout << "- 1/2: 调整环境光强度" << std::endl;
    std::cout << "- 3/4: 调整漫反射强度" << std::endl;
    std::cout << "- 5/6: 调整镜面反射强度" << std::endl;
    std::cout << "- 7/8: 调整反光度" << std::endl;
    std::cout << "- R: 重置参数" << std::endl;
    std::cout << "- ESC: 退出" << std::endl;
    std::cout << "=========================" << std::endl;

    // 渲染循环
    while (!glfwWindowShouldClose(window))
    {
        // 计算时间
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // 输入处理
        processInput(window);

        // 动态光源移动
        if (lightMoving) {
            float time = glfwGetTime();
            lightPos.x = 3.0f * cos(time * lightSpeed);
            lightPos.z = 3.0f * sin(time * lightSpeed);
            lightPos.y = 2.0f + sin(time * lightSpeed * 0.5f);
        }

        // 清除缓冲
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 激活Phong着色器
        phongShader.use();
        
        // 设置光照参数
        phongShader.setVec3("objectColor", objectColor);
        phongShader.setVec3("lightColor", lightColor);
        phongShader.setVec3("lightPos", lightPos);
        phongShader.setVec3("viewPos", camera.getPosition());
        phongShader.setFloat("ambientStrength", ambientStrength);
        phongShader.setFloat("diffuseStrength", diffuseStrength);
        phongShader.setFloat("specularStrength", specularStrength);
        phongShader.setFloat("shininess", shininess);

        // 设置变换矩阵
        glm::mat4 projection = camera.getProjectionMatrix((float)SCR_WIDTH / (float)SCR_HEIGHT);
        glm::mat4 view = camera.getViewMatrix();
        phongShader.setMat4("projection", projection);
        phongShader.setMat4("view", view);

        // 绘制主立方体
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        phongShader.setMat4("model", model);
        cubeMesh.draw();

        // 激活光源着色器
        lightShader.use();
        lightShader.setVec3("lightColor", lightColor);
        lightShader.setMat4("projection", projection);
        lightShader.setMat4("view", view);

        // 绘制光源
        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f));
        lightShader.setMat4("model", model);
        cubeMesh.draw();

        // 交换缓冲并检查事件
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // 清理资源
    glfwTerminate();
    return 0;
}

// 处理输入
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    camera.processKeyboard(window, deltaTime);
}

// 键盘回调
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS) {
        switch (key) {
            case GLFW_KEY_SPACE:
                lightMoving = !lightMoving;
                std::cout << "光源移动: " << (lightMoving ? "开启" : "关闭") << std::endl;
                break;
            case GLFW_KEY_1:
                ambientStrength = std::max(0.0f, ambientStrength - 0.1f);
                std::cout << "环境光强度: " << ambientStrength << std::endl;
                break;
            case GLFW_KEY_2:
                ambientStrength = std::min(1.0f, ambientStrength + 0.1f);
                std::cout << "环境光强度: " << ambientStrength << std::endl;
                break;
            case GLFW_KEY_3:
                diffuseStrength = std::max(0.0f, diffuseStrength - 0.1f);
                std::cout << "漫反射强度: " << diffuseStrength << std::endl;
                break;
            case GLFW_KEY_4:
                diffuseStrength = std::min(2.0f, diffuseStrength + 0.1f);
                std::cout << "漫反射强度: " << diffuseStrength << std::endl;
                break;
            case GLFW_KEY_5:
                specularStrength = std::max(0.0f, specularStrength - 0.1f);
                std::cout << "镜面反射强度: " << specularStrength << std::endl;
                break;
            case GLFW_KEY_6:
                specularStrength = std::min(2.0f, specularStrength + 0.1f);
                std::cout << "镜面反射强度: " << specularStrength << std::endl;
                break;
            case GLFW_KEY_7:
                shininess = std::max(1.0f, shininess - 8.0f);
                std::cout << "反光度: " << shininess << std::endl;
                break;
            case GLFW_KEY_8:
                shininess = std::min(256.0f, shininess + 8.0f);
                std::cout << "反光度: " << shininess << std::endl;
                break;
            case GLFW_KEY_R:
                // 重置参数
                ambientStrength = 0.1f;
                diffuseStrength = 1.0f;
                specularStrength = 0.5f;
                shininess = 32.0f;
                objectColor = glm::vec3(1.0f, 0.5f, 0.31f);
                lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
                std::cout << "参数已重置" << std::endl;
                break;
        }
    }
}

// 窗口大小改变回调
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// 鼠标移动回调
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    camera.processMouseMovement(xoffset, yoffset);
}

// 滚轮回调
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.processMouseScroll(static_cast<float>(yoffset));
} 