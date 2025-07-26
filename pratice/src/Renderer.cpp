#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Renderer.h"

#include <iostream>

struct GLFWwindow;

Renderer::Renderer(int width, int height, const char* title) {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        m_window = nullptr;
        return;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    m_window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!m_window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(m_window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        glfwDestroyWindow(m_window);
        m_window = nullptr;
        return;
    }
}

Renderer::~Renderer() {
    if (m_window) {
        glfwDestroyWindow(m_window);
    }
    glfwTerminate();
}

void Renderer::run(const std::function<void()>& renderFunc) {
    while (m_window && !glfwWindowShouldClose(m_window)) {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        renderFunc();
        glfwSwapBuffers(m_window);
        glfwPollEvents();
    }
}

GLFWwindow* Renderer::window() const {
    return m_window;
} 