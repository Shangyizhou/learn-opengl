#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>
class Shader {
public:
    Shader(const char* vertexSrc, const char* fragmentSrc);
    ~Shader();
    void use() const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setVec3(const std::string& name, const glm::vec3& value) const;
    void setMat4(const std::string& name, const glm::mat4& mat) const;
    GLuint ID() const;
private:
    GLuint programID;
};

#endif // SHADER_H 