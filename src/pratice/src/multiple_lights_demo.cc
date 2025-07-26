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
const int NR_POINT_LIGHTS = 4;
glm::vec3 pointLightPositions[NR_POINT_LIGHTS] = {
    glm::vec3( 0.7f,  0.2f,  2.0f),
    glm::vec3( 2.3f, -3.3f, -4.0f),
    glm::vec3(-4.0f,  2.0f, -12.0f),
    glm::vec3( 0.0f,  0.0f, -3.0f)
};

glm::vec3 pointLightColors[NR_POINT_LIGHTS] = {
    glm::vec3(1.0f, 0.0f, 0.0f),   // 红色
    glm::vec3(0.0f, 1.0f, 0.0f),   // 绿色
    glm::vec3(0.0f, 0.0f, 1.0f),   // 蓝色
    glm::vec3(1.0f, 1.0f, 0.0f)    // 黄色
};

// 聚光参数
glm::vec3 spotLightPos(0.0f, 3.0f, 0.0f);
glm::vec3 spotLightDir(0.0f, -1.0f, 0.0f);
float cutOff = glm::cos(glm::radians(12.5f));
float outerCutOff = glm::cos(glm::radians(17.5f));

// 光源移动
bool lightMoving = true;
float lightSpeed = 0.5f;

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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Multiple Lights Demo", NULL, NULL);
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

    // 创建多光源着色器
    Shader multipleLightsShader(
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
        // 片段着色器 - 多光源系统
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
        "// 材质\n"
        "struct Material {\n"
        "    vec3 ambient;\n"
        "    vec3 diffuse;\n"
        "    vec3 specular;\n"
        "    float shininess;\n"
        "};\n"
        "uniform Material material;\n"
        "\n"
        "// 平行光\n"
        "struct DirLight {\n"
        "    vec3 direction;\n"
        "    vec3 ambient;\n"
        "    vec3 diffuse;\n"
        "    vec3 specular;\n"
        "};\n"
        "uniform DirLight dirLight;\n"
        "\n"
        "// 点光源\n"
        "struct PointLight {\n"
        "    vec3 position;\n"
        "    vec3 ambient;\n"
        "    vec3 diffuse;\n"
        "    vec3 specular;\n"
        "    float constant;\n"
        "    float linear;\n"
        "    float quadratic;\n"
        "};\n"
        "uniform PointLight pointLights[4];\n"
        "\n"
        "// 聚光\n"
        "struct SpotLight {\n"
        "    vec3 position;\n"
        "    vec3 direction;\n"
        "    vec3 ambient;\n"
        "    vec3 diffuse;\n"
        "    vec3 specular;\n"
        "    float cutOff;\n"
        "    float outerCutOff;\n"
        "    float constant;\n"
        "    float linear;\n"
        "    float quadratic;\n"
        "};\n"
        "uniform SpotLight spotLight;\n"
        "\n"
        "// 函数声明\n"
        "vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);\n"
        "vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);\n"
        "vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);\n"
        "\n"
        "void main()\n"
        "{\n"
        "    vec3 norm = normalize(Normal);\n"
        "    vec3 viewDir = normalize(viewPos - FragPos);\n"
        "\n"
        "    // 第一阶段：平行光\n"
        "    vec3 result = CalcDirLight(dirLight, norm, viewDir);\n"
        "\n"
        "    // 第二阶段：点光源\n"
        "    for(int i = 0; i < 4; i++)\n"
        "        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);\n"
        "\n"
        "    // 第三阶段：聚光\n"
        "    result += CalcSpotLight(spotLight, norm, FragPos, viewDir);\n"
        "\n"
        "    FragColor = vec4(result * objectColor, 1.0);\n"
        "}\n"
        "\n"
        "vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)\n"
        "{\n"
        "    vec3 lightDir = normalize(-light.direction);\n"
        "    float diff = max(dot(normal, lightDir), 0.0);\n"
        "    vec3 reflectDir = reflect(-lightDir, normal);\n"
        "    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);\n"
        "    \n"
        "    vec3 ambient = light.ambient * material.ambient;\n"
        "    vec3 diffuse = light.diffuse * diff * material.diffuse;\n"
        "    vec3 specular = light.specular * spec * material.specular;\n"
        "    \n"
        "    return (ambient + diffuse + specular);\n"
        "}\n"
        "\n"
        "vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)\n"
        "{\n"
        "    vec3 lightDir = normalize(light.position - fragPos);\n"
        "    float diff = max(dot(normal, lightDir), 0.0);\n"
        "    vec3 reflectDir = reflect(-lightDir, normal);\n"
        "    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);\n"
        "    \n"
        "    float distance = length(light.position - fragPos);\n"
        "    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);\n"
        "    \n"
        "    vec3 ambient = light.ambient * material.ambient;\n"
        "    vec3 diffuse = light.diffuse * diff * material.diffuse;\n"
        "    vec3 specular = light.specular * spec * material.specular;\n"
        "    \n"
        "    ambient *= attenuation;\n"
        "    diffuse *= attenuation;\n"
        "    specular *= attenuation;\n"
        "    \n"
        "    return (ambient + diffuse + specular);\n"
        "}\n"
        "\n"
        "vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)\n"
        "{\n"
        "    vec3 lightDir = normalize(light.position - fragPos);\n"
        "    float diff = max(dot(normal, lightDir), 0.0);\n"
        "    vec3 reflectDir = reflect(-lightDir, normal);\n"
        "    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);\n"
        "    \n"
        "    float distance = length(light.position - fragPos);\n"
        "    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);\n"
        "    \n"
        "    float theta = dot(lightDir, normalize(-light.direction));\n"
        "    float epsilon = light.cutOff - light.outerCutOff;\n"
        "    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);\n"
        "    \n"
        "    vec3 ambient = light.ambient * material.ambient;\n"
        "    vec3 diffuse = light.diffuse * diff * material.diffuse;\n"
        "    vec3 specular = light.specular * spec * material.specular;\n"
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
    
    std::cout << "=== 多光源演示 ===" << std::endl;
    std::cout << "控制说明：" << std::endl;
    std::cout << "- WASD: 移动相机" << std::endl;
    std::cout << "- 鼠标: 旋转视角" << std::endl;
    std::cout << "- 滚轮: 缩放" << std::endl;
    std::cout << "- 空格: 切换光源移动" << std::endl;
    std::cout << "- 1/2: 调整聚光内角" << std::endl;
    std::cout << "- 3/4: 调整聚光外角" << std::endl;
    std::cout << "- R: 重置参数" << std::endl;
    std::cout << "- ESC: 退出" << std::endl;
    std::cout << "=================" << std::endl;
    std::cout << "场景包含：" << std::endl;
    std::cout << "- 1个平行光 (太阳光)" << std::endl;
    std::cout << "- 4个点光源 (彩色灯泡)" << std::endl;
    std::cout << "- 1个聚光 (手电筒)" << std::endl;
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
            
            // 移动点光源
            for (int i = 0; i < NR_POINT_LIGHTS; i++) {
                float offset = i * 1.57f; // 90度间隔
                pointLightPositions[i].x = 3.0f * cos(time * lightSpeed + offset);
                pointLightPositions[i].z = 3.0f * sin(time * lightSpeed + offset);
                pointLightPositions[i].y = 1.0f + sin(time * lightSpeed * 0.5f + offset);
            }
            
            // 移动聚光
            spotLightPos.x = 2.0f * cos(time * lightSpeed * 0.7f);
            spotLightPos.z = 2.0f * sin(time * lightSpeed * 0.7f);
        }

        // 清除缓冲
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 激活多光源着色器
        multipleLightsShader.use();
        
        // 设置通用参数
        multipleLightsShader.setVec3("viewPos", camera.getPosition());
        multipleLightsShader.setVec3("objectColor", glm::vec3(1.0f, 0.5f, 0.31f));

        // 设置材质
        multipleLightsShader.setVec3("material.ambient", glm::vec3(0.1f, 0.1f, 0.1f));
        multipleLightsShader.setVec3("material.diffuse", glm::vec3(0.7f, 0.7f, 0.7f));
        multipleLightsShader.setVec3("material.specular", glm::vec3(1.0f, 1.0f, 1.0f));
        multipleLightsShader.setFloat("material.shininess", 32.0f);

        // 设置平行光
        multipleLightsShader.setVec3("dirLight.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
        multipleLightsShader.setVec3("dirLight.ambient", glm::vec3(0.05f, 0.05f, 0.05f));
        multipleLightsShader.setVec3("dirLight.diffuse", glm::vec3(0.4f, 0.4f, 0.4f));
        multipleLightsShader.setVec3("dirLight.specular", glm::vec3(0.5f, 0.5f, 0.5f));

        // 设置点光源
        for (int i = 0; i < NR_POINT_LIGHTS; i++) {
            std::string prefix = "pointLights[" + std::to_string(i) + "].";
            multipleLightsShader.setVec3(prefix + "position", pointLightPositions[i]);
            multipleLightsShader.setVec3(prefix + "ambient", pointLightColors[i] * 0.1f);
            multipleLightsShader.setVec3(prefix + "diffuse", pointLightColors[i] * 0.8f);
            multipleLightsShader.setVec3(prefix + "specular", pointLightColors[i]);
            multipleLightsShader.setFloat(prefix + "constant", 1.0f);
            multipleLightsShader.setFloat(prefix + "linear", 0.09f);
            multipleLightsShader.setFloat(prefix + "quadratic", 0.032f);
        }

        // 设置聚光
        multipleLightsShader.setVec3("spotLight.position", spotLightPos);
        multipleLightsShader.setVec3("spotLight.direction", spotLightDir);
        multipleLightsShader.setVec3("spotLight.ambient", glm::vec3(0.0f, 0.0f, 0.0f));
        multipleLightsShader.setVec3("spotLight.diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
        multipleLightsShader.setVec3("spotLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
        multipleLightsShader.setFloat("spotLight.cutOff", cutOff);
        multipleLightsShader.setFloat("spotLight.outerCutOff", outerCutOff);
        multipleLightsShader.setFloat("spotLight.constant", 1.0f);
        multipleLightsShader.setFloat("spotLight.linear", 0.09f);
        multipleLightsShader.setFloat("spotLight.quadratic", 0.032f);

        // 设置变换矩阵
        glm::mat4 projection = camera.getProjectionMatrix((float)SCR_WIDTH / (float)SCR_HEIGHT);
        glm::mat4 view = camera.getViewMatrix();
        multipleLightsShader.setMat4("projection", projection);
        multipleLightsShader.setMat4("view", view);

        // 绘制多个立方体
        glm::mat4 model = glm::mat4(1.0f);
        
        // 主立方体
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        multipleLightsShader.setMat4("model", model);
        cubeMesh.draw();

        // 左侧立方体
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-2.0f, 0.0f, 0.0f));
        multipleLightsShader.setMat4("model", model);
        cubeMesh.draw();

        // 右侧立方体
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
        multipleLightsShader.setMat4("model", model);
        cubeMesh.draw();

        // 后方立方体
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, -2.0f));
        multipleLightsShader.setMat4("model", model);
        cubeMesh.draw();

        // 激活光源着色器
        lightShader.use();
        lightShader.setMat4("projection", projection);
        lightShader.setMat4("view", view);

        // 绘制点光源
        for (int i = 0; i < NR_POINT_LIGHTS; i++) {
            model = glm::mat4(1.0f);
            model = glm::translate(model, pointLightPositions[i]);
            model = glm::scale(model, glm::vec3(0.2f));
            lightShader.setVec3("lightColor", pointLightColors[i]);
            lightShader.setMat4("model", model);
            cubeMesh.draw();
        }

        // 绘制聚光光源
        model = glm::mat4(1.0f);
        model = glm::translate(model, spotLightPos);
        model = glm::scale(model, glm::vec3(0.2f));
        lightShader.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
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
                cutOff = std::max(0.0f, cutOff - 0.01f);
                std::cout << "聚光内角: " << glm::degrees(glm::acos(cutOff)) << "°" << std::endl;
                break;
            case GLFW_KEY_2:
                cutOff = std::min(1.0f, cutOff + 0.01f);
                std::cout << "聚光内角: " << glm::degrees(glm::acos(cutOff)) << "°" << std::endl;
                break;
            case GLFW_KEY_3:
                outerCutOff = std::max(0.0f, outerCutOff - 0.01f);
                std::cout << "聚光外角: " << glm::degrees(glm::acos(outerCutOff)) << "°" << std::endl;
                break;
            case GLFW_KEY_4:
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