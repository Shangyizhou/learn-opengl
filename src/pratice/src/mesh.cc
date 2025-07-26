#include "mesh.h"
#include <vector>
#include <cstddef>
#include <memory>

// VertexLayout 实现
VertexLayout::VertexLayout(std::initializer_list<VertexAttributeDesc> attrs, GLsizei stride)
    : attributes(attrs), stride(stride) {}

// VertexArray 实现
VertexArray::VertexArray() { glGenVertexArrays(1, &ID); }
VertexArray::~VertexArray() { glDeleteVertexArrays(1, &ID); }
void VertexArray::bind() const { glBindVertexArray(ID); }
void VertexArray::unbind() const { glBindVertexArray(0); }
void VertexArray::setLayout(const VertexLayout& layout) {
    for (const auto& attr : layout.attributes) {
        glEnableVertexAttribArray(attr.index);
        glVertexAttribPointer(attr.index, attr.size, attr.type, attr.normalized, layout.stride, (void*)attr.offset);
    }
}

// VertexBuffer 实现
VertexBuffer::VertexBuffer(const void* data, GLsizeiptr size) {
    glGenBuffers(1, &ID);
    glBindBuffer(GL_ARRAY_BUFFER, ID);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}
VertexBuffer::~VertexBuffer() { glDeleteBuffers(1, &ID); }
void VertexBuffer::bind() const { glBindBuffer(GL_ARRAY_BUFFER, ID); }
void VertexBuffer::unbind() const { glBindBuffer(GL_ARRAY_BUFFER, 0); }

// ElementBuffer 实现
ElementBuffer::ElementBuffer(const void* data, GLsizeiptr size) {
    glGenBuffers(1, &ID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}
ElementBuffer::~ElementBuffer() { glDeleteBuffers(1, &ID); }
void ElementBuffer::bind() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID); }
void ElementBuffer::unbind() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); }

// Mesh 实现
Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices)
    : vbo(vertices.data(), vertices.size() * sizeof(Vertex)),
      ebo(indices.data(), indices.size() * sizeof(uint32_t)),
      indexCount(indices.size())
{
    vao.bind();
    vbo.bind();
    ebo.bind();
    vao.setLayout(getLayout());
    vao.unbind();
}
void Mesh::draw() const {
    vao.bind();
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indexCount), GL_UNSIGNED_INT, 0);
}

void Mesh::drawLines() const {
    vao.bind();
    glDrawElements(GL_LINES, static_cast<GLsizei>(indexCount), GL_UNSIGNED_INT, 0);
}
VertexLayout Mesh::getLayout() {
    return VertexLayout{
        {
            {0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, position)},
            {1, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, color)},
            {2, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, normal)},
            {3, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, texcoord)}
        },
        sizeof(Vertex)
    };
}
