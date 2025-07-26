#pragma once
#include <string>
#include <unordered_map>
#include <glad/glad.h>
#include <glm/glm.hpp>

class Shader {
public:
    // 构造函数 - 从文件路径加载
    Shader(const std::string& vertexPath, const std::string& fragmentPath);
    // 构造函数 - 从源码字符串创建
    Shader(const std::string& vertexSource, const std::string& fragmentSource, bool fromSource);
    // 拷贝构造函数（删除）
    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;
    // 移动构造函数
    Shader(Shader&& other) noexcept;
    Shader& operator=(Shader&& other) noexcept;
    
    ~Shader();

    // 基本操作
    void use() const;
    bool isValid() const { return programID != 0 && compiled; }
    GLuint ID() const { return programID; }

    // Uniform设置方法（带缓存优化）
    void setBool(const std::string& name, bool value);
    void setInt(const std::string& name, int value);
    void setFloat(const std::string& name, float value);
    void setVec2(const std::string& name, const glm::vec2& value);
    void setVec3(const std::string& name, const glm::vec3& value);
    void setVec4(const std::string& name, const glm::vec4& value);
    void setMat2(const std::string& name, const glm::mat2& mat);
    void setMat3(const std::string& name, const glm::mat3& mat);
    void setMat4(const std::string& name, const glm::mat4& mat);

    // 批量设置uniform（性能优化）
    void setUniforms(const std::unordered_map<std::string, int>& intUniforms);
    void setUniforms(const std::unordered_map<std::string, float>& floatUniforms);
    void setUniforms(const std::unordered_map<std::string, glm::vec3>& vec3Uniforms);
    void setUniforms(const std::unordered_map<std::string, glm::mat4>& mat4Uniforms);

    // 工具方法
    void printActiveUniforms() const;  // 调试用
    bool hasUniform(const std::string& name) const;
    void reload();  // 重新加载着色器（调试用）

private:
    GLuint programID = 0;
    bool compiled = false;
    std::string vertexPath, fragmentPath;  // 保存路径以便重新加载
    
    // Uniform位置缓存（性能优化）
    mutable std::unordered_map<std::string, GLint> uniformCache;

    // 内部方法
    static std::string loadFile(const std::string& path);
    bool compileShader(const std::string& source, GLenum shaderType, GLuint& shaderID);
    bool linkProgram(GLuint vertexShader, GLuint fragmentShader);
    GLint getUniformLocation(const std::string& name) const;
    void checkCompileErrors(GLuint shader, const std::string& type);
}; 