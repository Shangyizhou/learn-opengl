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
Camera camera(glm::vec3(0.0f, 2.0f, 8.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// 时间
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// 光源参数
glm::vec3 pointLightPos(2.0f, 3.0f, 2.0f);
glm::vec3 spotLightPos(0.0f, 3.0f, 0.0f);
glm::vec3 spotLightDir(0.0f, -1.0f, 0.0f);
bool lightMoving = true;
float lightSpeed = 1.0f;

// 聚光参数
float cutOff = glm::cos(glm::radians(12.5f));
float outerCutOff = glm::cos(glm::radians(17.5f));

// 光照类型
enum LightType {
    DIRECTIONAL_LIGHT = 0,
    POINT_LIGHT = 1,
    SPOT_LIGHT = 2
};
LightType currentLightType = DIRECTIONAL_LIGHT;

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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Light Casters Demo", NULL, NULL);
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

    // 创建投光物着色器
    Shader lightCastersShader(
        // 顶点着色器
        "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "layout (location = 1) in vec3 aColor;\n"
        "layout (location = 2) in vec3 aNormal;\n"
        "layout (location = 3) in vec2 aTexCoord;\n"
        "\n"
        "out vec3 FragPos;\n"
        "out vec3 Normal;\n"
        "out vec2 TexCoords;\n"
        "\n"
        "uniform mat4 model;\n"
        "uniform mat4 view;\n"
        "uniform mat4 projection;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    FragPos = vec3(model * vec4(aPos, 1.0));\n"
        "    Normal = mat3(transpose(inverse(model))) * aNormal;\n"
        "    TexCoords = aTexCoord;\n"
        "    gl_Position = projection * view * vec4(FragPos, 1.0);\n"
        "}\n",
        // 片段着色器 - 支持三种投光物
        "#version 330 core\n"
        "out vec4 FragColor;\n"
        "\n"
        "in vec3 FragPos;\n"
        "in vec3 Normal;\n"
        "in vec2 TexCoords;\n"
        "\n"
        "uniform vec3 viewPos;\n"
        "uniform vec3 objectColor;\n"
        "\n"
        "// 平行光\n"
        "uniform vec3 dirLightDirection;\n"
        "uniform vec3 dirLightAmbient;\n"
        "uniform vec3 dirLightDiffuse;\n"
        "uniform vec3 dirLightSpecular;\n"
        "\n"
        "// 点光源\n"
        "uniform vec3 pointLightPos;\n"
        "uniform vec3 pointLightAmbient;\n"
        "uniform vec3 pointLightDiffuse;\n"
        "uniform vec3 pointLightSpecular;\n"
        "uniform float pointLightConstant;\n"
        "uniform float pointLightLinear;\n"
        "uniform float pointLightQuadratic;\n"
        "\n"
        "// 聚光\n"
        "uniform vec3 spotLightPos;\n"
        "uniform vec3 spotLightDirection;\n"
        "uniform vec3 spotLightAmbient;\n"
        "uniform vec3 spotLightDiffuse;\n"
        "uniform vec3 spotLightSpecular;\n"
        "uniform float spotLightCutOff;\n"
        "uniform float spotLightOuterCutOff;\n"
        "uniform float spotLightConstant;\n"
        "uniform float spotLightLinear;\n"
        "uniform float spotLightQuadratic;\n"
        "\n"
        "// 材质\n"
        "uniform float materialShininess;\n"
        "\n"
        "// 光照类型\n"
        "uniform int lightType;\n"
        "\n"
        "vec3 CalcDirLight(vec3 normal, vec3 viewDir);\n"
        "vec3 CalcPointLight(vec3 normal, vec3 fragPos, vec3 viewDir);\n"
        "vec3 CalcSpotLight(vec3 normal, vec3 fragPos, vec3 viewDir);\n"
        "\n"
        "void main()\n"
        "{\n"
        "    vec3 norm = normalize(Normal);\n"
        "    vec3 viewDir = normalize(viewPos - FragPos);\n"
        "    vec3 result = vec3(0.0);\n"
        "\n"
        "    if(lightType == 0) {\n"
        "        result = CalcDirLight(norm, viewDir);\n"
        "    } else if(lightType == 1) {\n"
        "        result = CalcPointLight(norm, FragPos, viewDir);\n"
        "    } else if(lightType == 2) {\n"
        "        result = CalcSpotLight(norm, FragPos, viewDir);\n"
        "    }\n"
        "\n"
        "    FragColor = vec4(result * objectColor, 1.0);\n"
        "}\n"
        "\n"
        "vec3 CalcDirLight(vec3 normal, vec3 viewDir)\n"
        "{\n"
        "    vec3 lightDir = normalize(-dirLightDirection);\n"
        "    float diff = max(dot(normal, lightDir), 0.0);\n"
        "    vec3 reflectDir = reflect(-lightDir, normal);\n"
        "    float spec = pow(max(dot(viewDir, reflectDir), 0.0), materialShininess);\n"
        "    \n"
        "    vec3 ambient = dirLightAmbient;\n"
        "    vec3 diffuse = dirLightDiffuse * diff;\n"
        "    vec3 specular = dirLightSpecular * spec;\n"
        "    \n"
        "    return (ambient + diffuse + specular);\n"
        "}\n"
        "\n"
        "vec3 CalcPointLight(vec3 normal, vec3 fragPos, vec3 viewDir)\n"
        "{\n"
        "    vec3 lightDir = normalize(pointLightPos - fragPos);\n"
        "    float diff = max(dot(normal, lightDir), 0.0);\n"
        "    vec3 reflectDir = reflect(-lightDir, normal);\n"
        "    float spec = pow(max(dot(viewDir, reflectDir), 0.0), materialShininess);\n"
        "    \n"
        "    float distance = length(pointLightPos - fragPos);\n"
        "    float attenuation = 1.0 / (pointLightConstant + pointLightLinear * distance + pointLightQuadratic * distance * distance);\n"
        "    \n"
        "    vec3 ambient = pointLightAmbient;\n"
        "    vec3 diffuse = pointLightDiffuse * diff;\n"
        "    vec3 specular = pointLightSpecular * spec;\n"
        "    \n"
        "    ambient *= attenuation;\n"
        "    diffuse *= attenuation;\n"
        "    specular *= attenuation;\n"
        "    \n"
        "    return (ambient + diffuse + specular);\n"
        "}\n"
        "\n"
        "vec3 CalcSpotLight(vec3 normal, vec3 fragPos, vec3 viewDir)\n"
        "{\n"
        "    vec3 lightDir = normalize(spotLightPos - fragPos);\n"
        "    float diff = max(dot(normal, lightDir), 0.0);\n"
        "    vec3 reflectDir = reflect(-lightDir, normal);\n"
        "    float spec = pow(max(dot(viewDir, reflectDir), 0.0), materialShininess);\n"
        "    \n"
        "    float distance = length(spotLightPos - fragPos);\n"
        "    float attenuation = 1.0 / (spotLightConstant + spotLightLinear * distance + spotLightQuadratic * distance * distance);\n"
        "    \n"
        "    float theta = dot(lightDir, normalize(-spotLightDirection));\n"
        "    float epsilon = spotLightCutOff - spotLightOuterCutOff;\n"
        "    float intensity = clamp((theta - spotLightOuterCutOff) / epsilon, 0.0, 1.0);\n"
        "    \n"
        "    vec3 ambient = spotLightAmbient;\n"
        "    vec3 diffuse = spotLightDiffuse * diff;\n"
        "    vec3 specular = spotLightSpecular * spec;\n"
        "    \n"
        "    ambient *= attenuation * intensity;\n"
        "    diffuse *= attenuation * intensity;\n"
        "    specular *= attenuation * intensity;\n"
        "    \n"
        "    return (ambient + diffuse + specular);\n"
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
    
    std::cout << "=== 投光物演示 ===" << std::endl;
    std::cout << "控制说明：" << std::endl;
    std::cout << "- WASD: 移动相机" << std::endl;
    std::cout << "- 鼠标: 旋转视角" << std::endl;
    std::cout << "- 滚轮: 缩放" << std::endl;
    std::cout << "- 空格: 切换光源移动" << std::endl;
    std::cout << "- 1: 平行光" << std::endl;
    std::cout << "- 2: 点光源" << std::endl;
    std::cout << "- 3: 聚光" << std::endl;
    std::cout << "- 4/5: 调整聚光内角" << std::endl;
    std::cout << "- 6/7: 调整聚光外角" << std::endl;
    std::cout << "- R: 重置参数" << std::endl;
    std::cout << "- ESC: 退出" << std::endl;
    std::cout << "=================" << std::endl;

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
            pointLightPos.x = 3.0f * cos(time * lightSpeed);
            pointLightPos.z = 3.0f * sin(time * lightSpeed);
            pointLightPos.y = 2.0f + sin(time * lightSpeed * 0.5f);
            
            spotLightPos.x = 2.0f * cos(time * lightSpeed * 0.7f);
            spotLightPos.z = 2.0f * sin(time * lightSpeed * 0.7f);
        }

        // 清除缓冲
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 激活投光物着色器
        lightCastersShader.use();
        
        // 设置通用参数
        lightCastersShader.setVec3("viewPos", camera.getPosition());
        lightCastersShader.setVec3("objectColor", glm::vec3(1.0f, 0.5f, 0.31f));
        lightCastersShader.setFloat("materialShininess", 32.0f);
        lightCastersShader.setInt("lightType", currentLightType);

        // 设置平行光参数
        lightCastersShader.setVec3("dirLightDirection", glm::vec3(-0.2f, -1.0f, -0.3f));
        lightCastersShader.setVec3("dirLightAmbient", glm::vec3(0.05f, 0.05f, 0.05f));
        lightCastersShader.setVec3("dirLightDiffuse", glm::vec3(0.4f, 0.4f, 0.4f));
        lightCastersShader.setVec3("dirLightSpecular", glm::vec3(0.5f, 0.5f, 0.5f));

        // 设置点光源参数
        lightCastersShader.setVec3("pointLightPos", pointLightPos);
        lightCastersShader.setVec3("pointLightAmbient", glm::vec3(0.05f, 0.05f, 0.05f));
        lightCastersShader.setVec3("pointLightDiffuse", glm::vec3(0.8f, 0.8f, 0.8f));
        lightCastersShader.setVec3("pointLightSpecular", glm::vec3(1.0f, 1.0f, 1.0f));
        lightCastersShader.setFloat("pointLightConstant", 1.0f);
        lightCastersShader.setFloat("pointLightLinear", 0.09f);
        lightCastersShader.setFloat("pointLightQuadratic", 0.032f);

        // 设置聚光参数
        lightCastersShader.setVec3("spotLightPos", spotLightPos);
        lightCastersShader.setVec3("spotLightDirection", spotLightDir);
        lightCastersShader.setVec3("spotLightAmbient", glm::vec3(0.0f, 0.0f, 0.0f));
        lightCastersShader.setVec3("spotLightDiffuse", glm::vec3(1.0f, 1.0f, 1.0f));
        lightCastersShader.setVec3("spotLightSpecular", glm::vec3(1.0f, 1.0f, 1.0f));
        lightCastersShader.setFloat("spotLightCutOff", cutOff);
        lightCastersShader.setFloat("spotLightOuterCutOff", outerCutOff);
        lightCastersShader.setFloat("spotLightConstant", 1.0f);
        lightCastersShader.setFloat("spotLightLinear", 0.09f);
        lightCastersShader.setFloat("spotLightQuadratic", 0.032f);

        // 设置变换矩阵
        glm::mat4 projection = camera.getProjectionMatrix((float)SCR_WIDTH / (float)SCR_HEIGHT);
        glm::mat4 view = camera.getViewMatrix();
        lightCastersShader.setMat4("projection", projection);
        lightCastersShader.setMat4("view", view);

        // 绘制多个立方体
        glm::mat4 model = glm::mat4(1.0f);
        
        // 主立方体
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        lightCastersShader.setMat4("model", model);
        cubeMesh.draw();

        // 左侧立方体
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-2.0f, 0.0f, 0.0f));
        lightCastersShader.setMat4("model", model);
        cubeMesh.draw();

        // 右侧立方体
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
        lightCastersShader.setMat4("model", model);
        cubeMesh.draw();

        // 激活光源着色器
        lightShader.use();
        lightShader.setMat4("projection", projection);
        lightShader.setMat4("view", view);

        // 绘制点光源
        if (currentLightType == POINT_LIGHT) {
            model = glm::mat4(1.0f);
            model = glm::translate(model, pointLightPos);
            model = glm::scale(model, glm::vec3(0.2f));
            lightShader.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
            lightShader.setMat4("model", model);
            cubeMesh.draw();
        }

        // 绘制聚光光源
        if (currentLightType == SPOT_LIGHT) {
            model = glm::mat4(1.0f);
            model = glm::translate(model, spotLightPos);
            model = glm::scale(model, glm::vec3(0.2f));
            lightShader.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 0.0f));
            lightShader.setMat4("model", model);
            cubeMesh.draw();
        }

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
                currentLightType = DIRECTIONAL_LIGHT;
                std::cout << "切换到: 平行光" << std::endl;
                break;
            case GLFW_KEY_2:
                currentLightType = POINT_LIGHT;
                std::cout << "切换到: 点光源" << std::endl;
                break;
            case GLFW_KEY_3:
                currentLightType = SPOT_LIGHT;
                std::cout << "切换到: 聚光" << std::endl;
                break;
            case GLFW_KEY_4:
                cutOff = std::max(0.0f, cutOff - 0.01f);
                std::cout << "聚光内角: " << glm::degrees(glm::acos(cutOff)) << "°" << std::endl;
                break;
            case GLFW_KEY_5:
                cutOff = std::min(1.0f, cutOff + 0.01f);
                std::cout << "聚光内角: " << glm::degrees(glm::acos(cutOff)) << "°" << std::endl;
                break;
            case GLFW_KEY_6:
                outerCutOff = std::max(0.0f, outerCutOff - 0.01f);
                std::cout << "聚光外角: " << glm::degrees(glm::acos(outerCutOff)) << "°" << std::endl;
                break;
            case GLFW_KEY_7:
                outerCutOff = std::min(1.0f, outerCutOff + 0.01f);
                std::cout << "聚光外角: " << glm::degrees(glm::acos(outerCutOff)) << "°" << std::endl;
                break;
            case GLFW_KEY_R:
                // 重置参数
                cutOff = glm::cos(glm::radians(12.5f));
                outerCutOff = glm::cos(glm::radians(17.5f));
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