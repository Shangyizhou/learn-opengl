#ifndef RENDERER_H
#define RENDERER_H
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <functional>
#include <GLFW/glfw3.h>

class Renderer {
public:
    Renderer(int width, int height, const char* title);
    ~Renderer();
    void run(const std::function<void()>& renderFunc);

    GLFWwindow* window() const;
private:
    GLFWwindow* m_window;
};

#endif // RENDERER_H 