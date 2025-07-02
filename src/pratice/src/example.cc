
#include "Renderer.h"
#include "Shader.h"
#include "mesh.h"
#include <vector>

int main() {
    Renderer renderer(800, 600, "Mesh Demo");

    // 顶点数据（一个三角形，带颜色、法线、纹理坐标）
    std::vector<Vertex> vertices = {
        // 位置                // 颜色         // 法线         // 纹理坐标
        { { 0.5f, -0.5f, 0.0f }, {1,0,0}, {0,0,1}, {1,0} }, // 右下
        { {-0.5f, -0.5f, 0.0f }, {0,1,0}, {0,0,1}, {0,0} }, // 左下
        { { 0.0f,  0.5f, 0.0f }, {0,0,1}, {0,0,1}, {0.5,1} }  // 顶部
    };
    std::vector<uint32_t> indices = { 0, 1, 2 };
    Mesh mesh(vertices, indices);

    // 着色器源码
    const char* vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec3 aColor;
        layout (location = 2) in vec3 aNormal;
        layout (location = 3) in vec2 aTexCoord;
        out vec3 ourColor;
        void main() {
            gl_Position = vec4(aPos, 1.0);
            ourColor = aColor;
        }
    )";
    const char* fragmentShaderSource = R"(
        #version 330 core
        in vec3 ourColor;
        out vec4 FragColor;
        void main() {
            FragColor = vec4(ourColor, 1.0);
        }
    )";
    Shader shader(vertexShaderSource, fragmentShaderSource);

    renderer.run([&](){
        shader.use();
        mesh.draw();
    });

    return 0;
}
