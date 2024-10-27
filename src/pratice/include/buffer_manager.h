#include <iostream>
#include <memory>
#include <fstream>
#include <sstream>
#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

struct BufferManager {
    // 如果不传入 size_t size
    // 直接对 vertices 进行 sizeof 计算得到的是指针的大小而不是真实数组的大小
    BufferManager(float vertices[], size_t size) {
         unsigned int VBO, VAO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW); // 使用正确的大小

        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // color attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        VAO_ = VAO;
        VBO_ = VBO;
    }

    void release() {
        glDeleteVertexArrays(1, &VAO_);
        glDeleteBuffers(1, &VBO_);
    }

    unsigned int getVAO() { return VAO_; }
    unsigned int getVBO() { return VBO_; }

    unsigned int VAO_;
    unsigned int VBO_;
};