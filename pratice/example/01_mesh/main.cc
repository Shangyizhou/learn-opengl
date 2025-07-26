#include "Renderer.h"
#include "Shader.h"
#include "mesh.h"
#include "Texture.h"
#include "Camera.h"
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>
#include <iostream>

// 旋转角度变量
float rotateX = 0.0f;
float rotateY = 0.0f;

// 摄像机
Camera camera;
float lastX = 400.0f, lastY = 300.0f;
bool firstMouse = true;
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// 处理输入，控制旋转角度和摄像机移动
void processInput(GLFWwindow* window) {
    const float angleStep = 1.0f;
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)    rotateX -= angleStep;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)  rotateX += angleStep;
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)  rotateY -= angleStep;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) rotateY += angleStep;
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);

    camera.processKeyboard(window, deltaTime);
}

// 鼠标回调
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
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

// 滚轮回调
void scroll_callback(GLFWwindow* window, double /*xoffset*/, double yoffset) {
    camera.processMouseScroll(static_cast<float>(yoffset));
}

int main() {
    Renderer renderer(800, 600, "Mesh Camera Demo");
    glfwSetInputMode(renderer.window(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(renderer.window(), mouse_callback);
    glfwSetScrollCallback(renderer.window(), scroll_callback);

    std::vector<Vertex> vertices = {
        // 位置                // 颜色         // 法线         // 纹理坐标
        { { 0.5f, -0.5f, 0.0f }, {1,0,0}, {0,0,1}, {1,0} }, // 右下
        { {-0.5f, -0.5f, 0.0f }, {0,1,0}, {0,0,1}, {0,0} }, // 左下
        { { 0.0f,  0.5f, 0.0f }, {0,0,1}, {0,0,1}, {0.5,1} }  // 顶部
    };
    std::vector<uint32_t> indices = { 0, 1, 2 };
    Mesh mesh(vertices, indices);

    Shader shader("/home/shangyizhou/code/learn-opengl/src/pratice/src/glsl/texture.vs", "/home/shangyizhou/code/learn-opengl/src/pratice/src/glsl/texture.fs");
    Texture texture("/home/shangyizhou/code/learn-opengl/src/pratice/src/textures/container.jpg", GL_RGB);
    shader.use();
    shader.setInt("ourTexture", 0);

    renderer.run([&](){
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(renderer.window());

        texture.bind(0);
        shader.use();

        glm::mat4 view = camera.getViewMatrix();
        glm::mat4 projection = camera.getProjectionMatrix(800.0f/600.0f);

        shader.setMat4("view", view);
        shader.setMat4("projection", projection);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(rotateX), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(rotateY), glm::vec3(0.0f, 1.0f, 0.0f));

        GLuint modelLoc = glGetUniformLocation(shader.ID(), "transform");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        mesh.draw();
    });

    return 0;
}