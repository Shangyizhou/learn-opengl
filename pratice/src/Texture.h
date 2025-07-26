#pragma once
#include <string>
#include <glad/glad.h>

class Texture {
public:
    Texture(const std::string& path, GLenum format = GL_RGB, bool flip = true);
    ~Texture();
    void bind(GLuint unit = 0) const;
    GLuint id() const { return m_id; }
private:
    GLuint m_id;
}; 