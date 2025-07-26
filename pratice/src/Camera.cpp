#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
    : position(position), worldUp(up), yaw(yaw), pitch(pitch), fov(45.0f),
      movementSpeed(2.5f), mouseSensitivity(0.001f) // 修改这里
{
    updateCameraVectors();
}

glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(position, position + front, up);
}

glm::mat4 Camera::getProjectionMatrix(float aspect, float nearPlane, float farPlane) const {
    return glm::perspective(glm::radians(fov), aspect, nearPlane, farPlane);
}

void Camera::processKeyboard(GLFWwindow* window, float deltaTime) {
    float velocity = movementSpeed * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        position += front * velocity;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        position -= front * velocity;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        position -= right * velocity;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        position += right * velocity;
}

void Camera::processMouseMovement(float xoffset, float yoffset, bool constrainPitch) {
    xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;

    yaw   += xoffset;
    pitch += yoffset;

    if (constrainPitch) {
        if (pitch > 89.0f) pitch = 89.0f;
        if (pitch < -89.0f) pitch = -89.0f;
    }
    updateCameraVectors();
}

void Camera::processMouseScroll(float yoffset) {
    fov -= yoffset;
    if (fov < 1.0f) fov = 1.0f;
    if (fov > 45.0f) fov = 45.0f;
}

void Camera::updateCameraVectors() {
    // yaw和pitch分别是水平和垂直的旋转角度（单位：度）
    // 通过三角函数将球坐标(yaw, pitch, 半径1)转换为笛卡尔坐标(front向量)
    // yaw控制左右，pitch控制上下
    glm::vec3 f;
    f.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch)); // 前方在x轴的分量
    f.y = sin(glm::radians(pitch));                          // 前方在y轴的分量
    f.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch)); // 前方在z轴的分量
    front = glm::normalize(f);
    right = glm::normalize(glm::cross(front, worldUp));
    up    = glm::normalize(glm::cross(right, front));
}