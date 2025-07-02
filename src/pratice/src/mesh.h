#ifndef MESH_H
#define MESH_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <cstddef>
#include <memory>

// 顶点结构体，包含位置、颜色、法线、纹理坐标
struct Vertex {
    glm::vec3 position;
    glm::vec3 color;
    glm::vec3 normal;
    glm::vec2 texcoord;
};

// 顶点属性描述
struct VertexAttributeDesc {
    GLuint index;
    GLint size;
    GLenum type;
    GLboolean normalized;
    size_t offset;
};

// 顶点布局类
class VertexLayout {
public:
    std::vector<VertexAttributeDesc> attributes;
    GLsizei stride;
    VertexLayout(std::initializer_list<VertexAttributeDesc> attrs, GLsizei stride);
};

// VAO 封装
class VertexArray {
public:
    VertexArray();
    ~VertexArray();
    void bind() const;
    void unbind() const;
    void setLayout(const VertexLayout& layout);
private:
    GLuint ID;
};

// VBO 封装
class VertexBuffer {
public:
    VertexBuffer(const void* data, GLsizeiptr size);
    ~VertexBuffer();
    void bind() const;
    void unbind() const;
private:
    GLuint ID;
};

// EBO 封装
class ElementBuffer {
public:
    ElementBuffer(const void* data, GLsizeiptr size);
    ~ElementBuffer();
    void bind() const;
    void unbind() const;
private:
    GLuint ID;
};

// Mesh 封装
class Mesh {
public:
    Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
    void draw() const;
    static VertexLayout getLayout();
private:
    VertexArray vao;
    VertexBuffer vbo;
    ElementBuffer ebo;
    size_t indexCount;
};

#endif // MESH_H
