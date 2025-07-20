#include <glad/glad.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// 包含现有的模块头文件
#include "../Camera.h"
#include "../mesh.h"
#include "../Shader.h"
#include "../Texture.h"
#include "../Renderer.h"

// 全局变量
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = 400.0f, lastY = 300.0f;
bool firstMouse = true;
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// 相机控制变量
float cameraYaw = -90.0f;
float cameraPitch = 0.0f;
float cameraFov = 45.0f;
float cameraSpeed = 2.5f;
float mouseSensitivity = 0.001f;
glm::vec3 cameraPosition(0.0f, 0.0f, 3.0f);

// 模型变换变量
float rotateX = 0.0f;
float rotateY = 0.0f;
float rotateZ = 0.0f;
float scaleX = 1.0f;
float scaleY = 1.0f;
float scaleZ = 1.0f;
glm::vec3 modelPosition(0.0f, 0.0f, 0.0f);

// UI 控制变量
bool showCameraWindow = true;
bool showModelWindow = true;
bool showInfoWindow = true;
bool enableMouseControl = false;
bool enableKeyboardControl = true;

// 鼠标回调函数
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
    if (!enableMouseControl) return;
    
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse) {
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

// 滚轮回调函数
void scroll_callback(GLFWwindow* window, double /*xoffset*/, double yoffset) {
    camera.processMouseScroll(static_cast<float>(yoffset));
}

// 键盘处理函数
void processKeyboard(GLFWwindow* window) {
    if (!enableKeyboardControl) return;
    
    camera.processKeyboard(window, deltaTime);
}

int main() {
    // 初始化GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // 配置GLFW窗口
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // 创建窗口
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Camera Control ImGui Demo", nullptr, nullptr);
    if (window == nullptr) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // 启用垂直同步

    // 设置回调函数
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // 初始化GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // 启用深度测试
    glEnable(GL_DEPTH_TEST);

    // 初始化ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    io.Fonts->AddFontFromFileTTF("/usr/share/fonts/truetype/wqy/wqy-microhei.ttc", 18.0f, nullptr, io.Fonts->GetGlyphRangesChineseFull());


    // 设置ImGui后端
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // 创建三角形网格
    std::vector<Vertex> vertices = {
        // 位置                // 颜色         // 法线         // 纹理坐标
        { { 0.5f, -0.5f, 0.0f }, {1,0,0}, {0,0,1}, {1,0} }, // 右下
        { {-0.5f, -0.5f, 0.0f }, {0,1,0}, {0,0,1}, {0,0} }, // 左下
        { { 0.0f,  0.5f, 0.0f }, {0,0,1}, {0,0,1}, {0.5,1} }  // 顶部
    };
    std::vector<uint32_t> indices = { 0, 1, 2 };
    Mesh mesh(vertices, indices);

    // 创建着色器
    Shader shader("/home/shangyizhou/code/learn-opengl/src/pratice/src/glsl/texture.vs", 
                  "/home/shangyizhou/code/learn-opengl/src/pratice/src/glsl/texture.fs");
    shader.use();
    shader.setInt("ourTexture", 0);

    // 主循环
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glfwPollEvents();
        processKeyboard(window);

        // 开始新帧
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // 相机控制窗口
        if (showCameraWindow) {
            ImGui::Begin("相机控制", &showCameraWindow);
            
            ImGui::Text("相机位置");
            if (ImGui::SliderFloat3("位置", &cameraPosition.x, -10.0f, 10.0f)) {
                // 这里需要更新相机位置，但Camera类没有setPosition方法
                // 我们通过UI控制来更新全局变量
            }
            
            ImGui::Text("相机旋转");
            if (ImGui::SliderFloat("偏航角 (Yaw)", &cameraYaw, -180.0f, 180.0f)) {
                // 更新相机偏航角
            }
            if (ImGui::SliderFloat("俯仰角 (Pitch)", &cameraPitch, -89.0f, 89.0f)) {
                // 更新相机俯仰角
            }
            
            ImGui::Text("相机参数");
            if (ImGui::SliderFloat("视野 (FOV)", &cameraFov, 1.0f, 90.0f)) {
                // 更新相机FOV
            }
            if (ImGui::SliderFloat("移动速度", &cameraSpeed, 0.1f, 10.0f)) {
                // 更新相机移动速度
            }
            if (ImGui::SliderFloat("鼠标灵敏度", &mouseSensitivity, 0.0001f, 0.01f)) {
                // 更新鼠标灵敏度
            }
            
            ImGui::Separator();
            ImGui::Checkbox("启用鼠标控制", &enableMouseControl);
            ImGui::Checkbox("启用键盘控制", &enableKeyboardControl);
            
            if (ImGui::Button("重置相机")) {
                cameraPosition = glm::vec3(0.0f, 0.0f, 3.0f);
                cameraYaw = -90.0f;
                cameraPitch = 0.0f;
                cameraFov = 45.0f;
            }
            
            ImGui::End();
        }

        // 模型变换窗口
        if (showModelWindow) {
            ImGui::Begin("模型变换", &showModelWindow);
            
            ImGui::Text("位置");
            ImGui::SliderFloat3("模型位置", &modelPosition.x, -5.0f, 5.0f);
            
            ImGui::Text("旋转");
            ImGui::SliderFloat("X轴旋转", &rotateX, 0.0f, 360.0f);
            ImGui::SliderFloat("Y轴旋转", &rotateY, 0.0f, 360.0f);
            ImGui::SliderFloat("Z轴旋转", &rotateZ, 0.0f, 360.0f);
            
            ImGui::Text("缩放");
            ImGui::SliderFloat("X轴缩放", &scaleX, 0.1f, 3.0f);
            ImGui::SliderFloat("Y轴缩放", &scaleY, 0.1f, 3.0f);
            ImGui::SliderFloat("Z轴缩放", &scaleZ, 0.1f, 3.0f);
            
            if (ImGui::Button("重置变换")) {
                modelPosition = glm::vec3(0.0f, 0.0f, 0.0f);
                rotateX = rotateY = rotateZ = 0.0f;
                scaleX = scaleY = scaleZ = 1.0f;
            }
            
            ImGui::End();
        }

        // 信息显示窗口
        if (showInfoWindow) {
            ImGui::Begin("信息显示", &showInfoWindow);
            
            ImGui::Text("性能信息");
            ImGui::Text("帧率: %.1f FPS", io.Framerate);
            ImGui::Text("帧时间: %.3f ms", 1000.0f / io.Framerate);
            
            ImGui::Separator();
            ImGui::Text("相机信息");
            ImGui::Text("位置: (%.2f, %.2f, %.2f)", cameraPosition.x, cameraPosition.y, cameraPosition.z);
            ImGui::Text("偏航角: %.2f°", cameraYaw);
            ImGui::Text("俯仰角: %.2f°", cameraPitch);
            ImGui::Text("视野: %.2f°", cameraFov);
            
            ImGui::Separator();
            ImGui::Text("模型信息");
            ImGui::Text("位置: (%.2f, %.2f, %.2f)", modelPosition.x, modelPosition.y, modelPosition.z);
            ImGui::Text("旋转: (%.1f°, %.1f°, %.1f°)", rotateX, rotateY, rotateZ);
            ImGui::Text("缩放: (%.2f, %.2f, %.2f)", scaleX, scaleY, scaleZ);
            
            ImGui::End();
        }

        // 渲染3D场景
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();

        // 设置视图和投影矩阵
        glm::mat4 view = camera.getViewMatrix();
        glm::mat4 projection = camera.getProjectionMatrix(1280.0f/720.0f);
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);

        // 设置模型矩阵
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, modelPosition);
        model = glm::rotate(model, glm::radians(rotateX), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(rotateY), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(rotateZ), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, glm::vec3(scaleX, scaleY, scaleZ));
        
        GLuint modelLoc = glGetUniformLocation(shader.ID(), "transform");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        // 绘制网格
        mesh.draw();

        // 渲染ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // 清理
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
} 