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

// 扩展的相机控制器类，支持通过UI设置相机参数
class ExtendedCamera {
public:
    ExtendedCamera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f))
        : camera(position), position(position), yaw(-90.0f), pitch(0.0f), fov(45.0f) {
        updateCameraFromUI();
    }
    
    // 设置相机位置
    void setPosition(const glm::vec3& newPosition) {
        position = newPosition;
        updateCameraFromUI();
    }
    
    // 设置偏航角
    void setYaw(float newYaw) {
        yaw = newYaw;
        updateCameraFromUI();
    }
    
    // 设置俯仰角
    void setPitch(float newPitch) {
        pitch = newPitch;
        updateCameraFromUI();
    }
    
    // 设置视野
    void setFov(float newFov) {
        fov = newFov;
        updateCameraFromUI();
    }
    
    // 重置相机
    void reset() {
        position = glm::vec3(0.0f, 0.0f, 3.0f);
        yaw = -90.0f;
        pitch = 0.0f;
        fov = 45.0f;
        updateCameraFromUI();
    }
    
    // 获取相机对象
    Camera& getCamera() { return camera; }
    const Camera& getCamera() const { return camera; }
    
    // 获取当前参数
    glm::vec3 getPosition() const { return position; }
    float getYaw() const { return yaw; }
    float getPitch() const { return pitch; }
    float getFov() const { return fov; }
    
private:
    void updateCameraFromUI() {
        // 重新创建相机对象以应用新的参数
        camera = Camera(position, glm::vec3(0.0f, 1.0f, 0.0f), yaw, pitch);
        // 手动设置FOV（因为Camera构造函数没有FOV参数）
        // 注意：这里我们通过重新创建相机来应用参数
    }
    
    Camera camera;
    glm::vec3 position;
    float yaw;
    float pitch;
    float fov;
};

// 全局变量
ExtendedCamera extendedCamera(glm::vec3(0.0f, 0.0f, 10.0f));  // 初始位置更远
float lastX = 400.0f, lastY = 300.0f;
bool firstMouse = true;
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// 相机控制变量
float cameraSpeed = 2.5f;
float mouseSensitivity = 0.001f;

// 坐标轴可视化变量
bool showCoordinateAxes = true;
bool showCameraPosition = true;
bool showModelAxes = true;
bool showSceneBounds = true;
float axisLength = 1.0f;
float axisThickness = 2.0f;

// 模型变换变量
float rotateX = 0.0f;
float rotateY = 0.0f;
float rotateZ = 0.0f;
float scaleX = 1.0f;
float scaleY = 1.0f;
float scaleZ = 1.0f;
glm::vec3 modelPosition(0.0f, 0.0f, 0.0f);
bool continuousRotation = false;
float rotationSpeed = 30.0f; // 度/秒

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

    extendedCamera.getCamera().processMouseMovement(xoffset, yoffset);
}

// 滚轮回调函数
void scroll_callback(GLFWwindow* window, double /*xoffset*/, double yoffset) {
    extendedCamera.getCamera().processMouseScroll(static_cast<float>(yoffset));
}

// 键盘处理函数
void processKeyboard(GLFWwindow* window) {
    if (!enableKeyboardControl) return;
    
    extendedCamera.getCamera().processKeyboard(window, deltaTime);
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

    // 加载中文字体
    ImFont* font = io.Fonts->AddFontFromFileTTF("/usr/share/fonts/truetype/wqy/wqy-microhei.ttc", 16.0f, nullptr, io.Fonts->GetGlyphRangesChineseSimplifiedCommon());
    if (font == nullptr) {
        std::cerr << "Failed to load Chinese font, falling back to default font" << std::endl;
    }

    // 设置ImGui后端
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // 创建三角形网格（增大尺寸）
    std::vector<Vertex> vertices = {
        // 位置                // 颜色         // 法线         // 纹理坐标
        { { 2.0f, -2.0f, 0.0f }, {1,0,0}, {0,0,1}, {1,0} }, // 右下
        { {-2.0f, -2.0f, 0.0f }, {0,1,0}, {0,0,1}, {0,0} }, // 左下
        { { 0.0f,  2.0f, 0.0f }, {0,0,1}, {0,0,1}, {0.5,1} }  // 顶部
    };
    std::vector<uint32_t> indices = { 0, 1, 2 };
    Mesh mesh(vertices, indices);

    // 创建坐标轴网格
    // 世界坐标轴（原点）
    std::vector<Vertex> worldAxisVertices = {
        // X轴 (红色)
        { { 0.0f, 0.0f, 0.0f }, {1.0f, 0.0f, 0.0f}, {0,0,1}, {0,0} },
        { { axisLength, 0.0f, 0.0f }, {1.0f, 0.0f, 0.0f}, {0,0,1}, {1,0} },
        // Y轴 (绿色)
        { { 0.0f, 0.0f, 0.0f }, {0.0f, 1.0f, 0.0f}, {0,0,1}, {0,0} },
        { { 0.0f, axisLength, 0.0f }, {0.0f, 1.0f, 0.0f}, {0,0,1}, {1,0} },
        // Z轴 (蓝色)
        { { 0.0f, 0.0f, 0.0f }, {0.0f, 0.0f, 1.0f}, {0,0,1}, {0,0} },
        { { 0.0f, 0.0f, axisLength }, {0.0f, 0.0f, 1.0f}, {0,0,1}, {1,0} }
    };
    std::vector<uint32_t> worldAxisIndices = { 0,1, 2,3, 4,5 };
    Mesh worldAxes(worldAxisVertices, worldAxisIndices);

    // 相机位置指示器（增大尺寸）
    std::vector<Vertex> cameraIndicatorVertices = {
        // 相机位置球体 (黄色)
        { { -0.3f, -0.3f, -0.3f }, {1.0f, 1.0f, 0.0f}, {0,0,1}, {0,0} },
        { {  0.3f, -0.3f, -0.3f }, {1.0f, 1.0f, 0.0f}, {0,0,1}, {1,0} },
        { {  0.3f,  0.3f, -0.3f }, {1.0f, 1.0f, 0.0f}, {0,0,1}, {1,1} },
        { { -0.3f,  0.3f, -0.3f }, {1.0f, 1.0f, 0.0f}, {0,0,1}, {0,1} },
        { { -0.3f, -0.3f,  0.3f }, {1.0f, 1.0f, 0.0f}, {0,0,1}, {0,0} },
        { {  0.3f, -0.3f,  0.3f }, {1.0f, 1.0f, 0.0f}, {0,0,1}, {1,0} },
        { {  0.3f,  0.3f,  0.3f }, {1.0f, 1.0f, 0.0f}, {0,0,1}, {1,1} },
        { { -0.3f,  0.3f,  0.3f }, {1.0f, 1.0f, 0.0f}, {0,0,1}, {0,1} }
    };
    std::vector<uint32_t> cameraIndicatorIndices = {
        0,1,2, 0,2,3,  // 前面
        4,6,5, 4,7,6,  // 后面
        0,4,1, 1,4,5,  // 下面
        2,6,3, 3,6,7,  // 上面
        0,3,4, 3,7,4,  // 左面
        1,5,2, 2,5,6   // 右面
    };
    Mesh cameraIndicator(cameraIndicatorVertices, cameraIndicatorIndices);

    // 创建场景边界指示器（网格地面）
    std::vector<Vertex> groundVertices;
    std::vector<uint32_t> groundIndices;
    float groundSize = 15.0f;
    float gridSpacing = 1.0f;
    
    // 创建网格顶点
    for (float x = -groundSize; x <= groundSize; x += gridSpacing) {
        for (float z = -groundSize; z <= groundSize; z += gridSpacing) {
            // 每个网格单元的两个三角形
            groundVertices.push_back({{x, 0.0f, z}, {0.3f, 0.3f, 0.3f}, {0,1,0}, {0,0}});
            groundVertices.push_back({{x + gridSpacing, 0.0f, z}, {0.3f, 0.3f, 0.3f}, {0,1,0}, {1,0}});
            groundVertices.push_back({{x, 0.0f, z + gridSpacing}, {0.3f, 0.3f, 0.3f}, {0,1,0}, {0,1}});
            groundVertices.push_back({{x + gridSpacing, 0.0f, z + gridSpacing}, {0.3f, 0.3f, 0.3f}, {0,1,0}, {1,1}});
            
            uint32_t baseIndex = groundVertices.size() - 4;
            groundIndices.push_back(baseIndex);
            groundIndices.push_back(baseIndex + 1);
            groundIndices.push_back(baseIndex + 2);
            groundIndices.push_back(baseIndex + 1);
            groundIndices.push_back(baseIndex + 3);
            groundIndices.push_back(baseIndex + 2);
        }
    }
    Mesh ground(groundVertices, groundIndices);

    // 创建着色器
    Shader shader("/home/shangyizhou/code/learn-opengl/src/pratice/src/glsl/texture.vs", 
                  "/home/shangyizhou/code/learn-opengl/src/pratice/src/glsl/texture.fs");
    shader.use();
    shader.setInt("ourTexture", 0);

    // 创建颜色着色器（用于坐标轴）
    Shader colorShader("/home/shangyizhou/code/learn-opengl/src/pratice/src/glsl/color.vs", 
                       "/home/shangyizhou/code/learn-opengl/src/pratice/src/glsl/color.fs");

    // 主循环
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glfwPollEvents();
        processKeyboard(window);

        // 连续旋转更新
        if (continuousRotation) {
            rotateY += rotationSpeed * deltaTime;
            if (rotateY >= 360.0f) rotateY -= 360.0f;
        }

        // 开始新帧
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // 相机控制窗口
        if (showCameraWindow) {
            ImGui::Begin("相机控制", &showCameraWindow);
            
            ImGui::Text("相机位置");
            glm::vec3 pos = extendedCamera.getPosition();
            if (ImGui::SliderFloat3("位置", &pos.x, -20.0f, 20.0f)) {
                extendedCamera.setPosition(pos);
            }
            
            ImGui::Text("相机旋转");
            float yaw = extendedCamera.getYaw();
            if (ImGui::SliderFloat("偏航角 (Yaw)", &yaw, -180.0f, 180.0f)) {
                extendedCamera.setYaw(yaw);
            }
            float pitch = extendedCamera.getPitch();
            if (ImGui::SliderFloat("俯仰角 (Pitch)", &pitch, -89.0f, 89.0f)) {
                extendedCamera.setPitch(pitch);
            }
            
            ImGui::Text("相机参数");
            float fov = extendedCamera.getFov();
            if (ImGui::SliderFloat("视野 (FOV)", &fov, 1.0f, 90.0f)) {
                extendedCamera.setFov(fov);
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
                extendedCamera.reset();
            }
            
            ImGui::SameLine();
            if (ImGui::Button("俯视视角")) {
                extendedCamera.setPosition(glm::vec3(0.0f, 15.0f, 0.0f));
                extendedCamera.setYaw(-90.0f);
                extendedCamera.setPitch(-90.0f);
            }
            
            ImGui::SameLine();
            if (ImGui::Button("侧视视角")) {
                extendedCamera.setPosition(glm::vec3(15.0f, 0.0f, 0.0f));
                extendedCamera.setYaw(180.0f);
                extendedCamera.setPitch(0.0f);
            }
            
            ImGui::Separator();
            ImGui::Text("演示说明:");
            ImGui::Text("• 模型旋转: 三角形围绕自身轴心旋转");
            ImGui::Text("• 相机旋转: 改变观察角度，就像转头");
            ImGui::Text("• 两者结合: 可以观察旋转中的模型");
            
            ImGui::End();
        }

        // 模型变换窗口
        if (showModelWindow) {
            ImGui::Begin("模型变换", &showModelWindow);
            
            ImGui::Text("位置");
            ImGui::SliderFloat3("模型位置", &modelPosition.x, -10.0f, 10.0f);
            
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
            
            ImGui::SameLine();
            if (ImGui::Button("连续旋转")) {
                continuousRotation = !continuousRotation;
            }
            
            if (continuousRotation) {
                ImGui::SliderFloat("旋转速度", &rotationSpeed, 10.0f, 100.0f);
            }
            
            ImGui::Separator();
            ImGui::Text("模型变换说明:");
            ImGui::Text("• X轴旋转: 前后翻转");
            ImGui::Text("• Y轴旋转: 左右翻转"); 
            ImGui::Text("• Z轴旋转: 平面旋转");
            
            ImGui::End();
        }

        // 坐标轴控制窗口
        ImGui::Begin("坐标轴显示", &showCoordinateAxes);
        
        ImGui::Checkbox("显示世界坐标轴", &showCoordinateAxes);
        ImGui::Checkbox("显示相机位置", &showCameraPosition);
        ImGui::Checkbox("显示模型坐标轴", &showModelAxes);
        ImGui::Checkbox("显示地面网格", &showSceneBounds);
        
        ImGui::SliderFloat("坐标轴长度", &axisLength, 0.5f, 3.0f);
        ImGui::SliderFloat("坐标轴粗细", &axisThickness, 1.0f, 5.0f);
        
        ImGui::Text("颜色说明:");
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "红色 = X轴");
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "绿色 = Y轴");
        ImGui::TextColored(ImVec4(0.0f, 0.0f, 1.0f, 1.0f), "蓝色 = Z轴");
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "黄色 = 相机位置");
        
        ImGui::End();

        // 信息显示窗口
        if (showInfoWindow) {
            ImGui::Begin("信息显示", &showInfoWindow);
            
            ImGui::Text("性能信息");
            ImGui::Text("帧率: %.1f FPS", io.Framerate);
            ImGui::Text("帧时间: %.3f ms", 1000.0f / io.Framerate);
            
            ImGui::Separator();
            ImGui::Text("相机信息");
            glm::vec3 pos = extendedCamera.getPosition();
            ImGui::Text("位置: (%.2f, %.2f, %.2f)", pos.x, pos.y, pos.z);
            ImGui::Text("偏航角: %.2f°", extendedCamera.getYaw());
            ImGui::Text("俯仰角: %.2f°", extendedCamera.getPitch());
            ImGui::Text("视野: %.2f°", extendedCamera.getFov());
            
            ImGui::Separator();
            ImGui::Text("模型信息");
            ImGui::Text("位置: (%.2f, %.2f, %.2f)", modelPosition.x, modelPosition.y, modelPosition.z);
            ImGui::Text("旋转: (%.1f°, %.1f°, %.1f°)", rotateX, rotateY, rotateZ);
            ImGui::Text("缩放: (%.2f, %.2f, %.2f)", scaleX, scaleY, scaleZ);
            
            ImGui::Separator();
            ImGui::Text("欧拉角说明:");
            ImGui::Text("模型欧拉角: 控制三角形自身的旋转");
            ImGui::Text("相机欧拉角: 控制观察者的视角");
            ImGui::Text("两者独立: 可以同时改变模型和视角");
            
            ImGui::End();
        }

        // 渲染3D场景
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();

        // 设置视图和投影矩阵
        glm::mat4 view = extendedCamera.getCamera().getViewMatrix();
        glm::mat4 projection = extendedCamera.getCamera().getProjectionMatrix(1280.0f/720.0f);
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);

        // 绘制地面网格
        if (showSceneBounds) {
            colorShader.use();
            colorShader.setMat4("view", view);
            colorShader.setMat4("projection", projection);
            
            glm::mat4 groundModel = glm::mat4(1.0f);
            colorShader.setMat4("transform", groundModel);
            ground.draw();
        }

        // 绘制世界坐标轴
        if (showCoordinateAxes) {
            glLineWidth(axisThickness);
            colorShader.use();
            colorShader.setMat4("view", view);
            colorShader.setMat4("projection", projection);
            
            glm::mat4 worldAxisModel = glm::mat4(1.0f);
            colorShader.setMat4("transform", worldAxisModel);
            worldAxes.drawLines();
            glLineWidth(1.0f);
        }

        // 绘制相机位置指示器
        if (showCameraPosition) {
            colorShader.use();
            colorShader.setMat4("view", view);
            colorShader.setMat4("projection", projection);
            
            glm::mat4 cameraModel = glm::mat4(1.0f);
            cameraModel = glm::translate(cameraModel, extendedCamera.getPosition());
            colorShader.setMat4("transform", cameraModel);
            cameraIndicator.draw();
        }

        // 绘制模型坐标轴
        if (showModelAxes) {
            glLineWidth(axisThickness);
            colorShader.use();
            colorShader.setMat4("view", view);
            colorShader.setMat4("projection", projection);
            
            glm::mat4 modelAxisModel = glm::mat4(1.0f);
            modelAxisModel = glm::translate(modelAxisModel, modelPosition);
            modelAxisModel = glm::rotate(modelAxisModel, glm::radians(rotateX), glm::vec3(1.0f, 0.0f, 0.0f));
            modelAxisModel = glm::rotate(modelAxisModel, glm::radians(rotateY), glm::vec3(0.0f, 1.0f, 0.0f));
            modelAxisModel = glm::rotate(modelAxisModel, glm::radians(rotateZ), glm::vec3(0.0f, 0.0f, 1.0f));
            colorShader.setMat4("transform", modelAxisModel);
            worldAxes.drawLines();
            glLineWidth(1.0f);
        }

        // 绘制主模型
        shader.use();
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);
        
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, modelPosition);
        model = glm::rotate(model, glm::radians(rotateX), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(rotateY), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(rotateZ), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, glm::vec3(scaleX, scaleY, scaleZ));
        
        shader.setMat4("transform", model);

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