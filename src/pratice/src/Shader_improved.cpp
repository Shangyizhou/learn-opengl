#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Shader.h"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <fstream>
#include <sstream>

std::string Shader::loadFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << path << std::endl;
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// 从文件路径加载的构造函数
Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath) 
    : vertexPath(vertexPath), fragmentPath(fragmentPath) {
    
    std::string vertexCode = loadFile(vertexPath);
    std::string fragmentCode = loadFile(fragmentPath);
    
    if (vertexCode.empty() || fragmentCode.empty()) {
        std::cerr << "ERROR::SHADER::FAILED_TO_LOAD_FILES" << std::endl;
        return;
    }

    GLuint vertexShader, fragmentShader;
    if (compileShader(vertexCode, GL_VERTEX_SHADER, vertexShader) &&
        compileShader(fragmentCode, GL_FRAGMENT_SHADER, fragmentShader)) {
        
        if (linkProgram(vertexShader, fragmentShader)) {
            compiled = true;
        }
    }
}

// 从源码字符串创建的构造函数
Shader::Shader(const std::string& vertexSource, const std::string& fragmentSource, bool fromSource) {
    if (!fromSource) {
        std::cerr << "ERROR::SHADER::INVALID_CONSTRUCTOR_USAGE" << std::endl;
        return;
    }

    GLuint vertexShader, fragmentShader;
    if (compileShader(vertexSource, GL_VERTEX_SHADER, vertexShader) &&
        compileShader(fragmentSource, GL_FRAGMENT_SHADER, fragmentShader)) {
        
        if (linkProgram(vertexShader, fragmentShader)) {
            compiled = true;
        }
    }
}

// 移动构造函数
Shader::Shader(Shader&& other) noexcept 
    : programID(other.programID), compiled(other.compiled),
      vertexPath(std::move(other.vertexPath)), fragmentPath(std::move(other.fragmentPath)),
      uniformCache(std::move(other.uniformCache)) {
    other.programID = 0;
    other.compiled = false;
}

// 移动赋值操作符
Shader& Shader::operator=(Shader&& other) noexcept {
    if (this != &other) {
        if (programID != 0) {
            glDeleteProgram(programID);
        }
        
        programID = other.programID;
        compiled = other.compiled;
        vertexPath = std::move(other.vertexPath);
        fragmentPath = std::move(other.fragmentPath);
        uniformCache = std::move(other.uniformCache);
        
        other.programID = 0;
        other.compiled = false;
    }
    return *this;
}

Shader::~Shader() {
    if (programID != 0) {
        glDeleteProgram(programID);
    }
}

void Shader::use() const {
    if (isValid()) {
        glUseProgram(programID);
    } else {
        std::cerr << "ERROR::SHADER::PROGRAM_NOT_VALID" << std::endl;
    }
}

// 编译着色器的内部方法
bool Shader::compileShader(const std::string& source, GLenum shaderType, GLuint& shaderID) {
    shaderID = glCreateShader(shaderType);
    const char* sourceCStr = source.c_str();
    glShaderSource(shaderID, 1, &sourceCStr, nullptr);
    glCompileShader(shaderID);
    
    checkCompileErrors(shaderID, (shaderType == GL_VERTEX_SHADER) ? "VERTEX" : "FRAGMENT");
    
    int success;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
    return success != 0;
}

// 链接程序的内部方法
bool Shader::linkProgram(GLuint vertexShader, GLuint fragmentShader) {
    programID = glCreateProgram();
    glAttachShader(programID, vertexShader);
    glAttachShader(programID, fragmentShader);
    glLinkProgram(programID);
    
    checkCompileErrors(programID, "PROGRAM");
    
    // 清理着色器对象
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    int success;
    glGetProgramiv(programID, GL_LINK_STATUS, &success);
    return success != 0;
}

// 获取uniform位置（带缓存）
GLint Shader::getUniformLocation(const std::string& name) const {
    auto it = uniformCache.find(name);
    if (it != uniformCache.end()) {
        return it->second;
    }
    
    GLint location = glGetUniformLocation(programID, name.c_str());
    if (location == -1) {
        std::cerr << "WARNING::SHADER::UNIFORM_NOT_FOUND: " << name << std::endl;
    }
    uniformCache[name] = location;
    return location;
}

// 错误检查方法
void Shader::checkCompileErrors(GLuint shader, const std::string& type) {
    int success;
    char infoLog[1024];
    
    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
            std::cerr << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" 
                      << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    } else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
            std::cerr << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" 
                      << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
}

// Uniform设置方法
void Shader::setBool(const std::string& name, bool value) {
    GLint location = getUniformLocation(name);
    if (location != -1) {
        glUniform1i(location, static_cast<int>(value));
    }
}

void Shader::setInt(const std::string& name, int value) {
    GLint location = getUniformLocation(name);
    if (location != -1) {
        glUniform1i(location, value);
    }
}

void Shader::setFloat(const std::string& name, float value) {
    GLint location = getUniformLocation(name);
    if (location != -1) {
        glUniform1f(location, value);
    }
}

void Shader::setVec2(const std::string& name, const glm::vec2& value) {
    GLint location = getUniformLocation(name);
    if (location != -1) {
        glUniform2fv(location, 1, glm::value_ptr(value));
    }
}

void Shader::setVec3(const std::string& name, const glm::vec3& value) {
    GLint location = getUniformLocation(name);
    if (location != -1) {
        glUniform3fv(location, 1, glm::value_ptr(value));
    }
}

void Shader::setVec4(const std::string& name, const glm::vec4& value) {
    GLint location = getUniformLocation(name);
    if (location != -1) {
        glUniform4fv(location, 1, glm::value_ptr(value));
    }
}

void Shader::setMat2(const std::string& name, const glm::mat2& mat) {
    GLint location = getUniformLocation(name);
    if (location != -1) {
        glUniformMatrix2fv(location, 1, GL_FALSE, glm::value_ptr(mat));
    }
}

void Shader::setMat3(const std::string& name, const glm::mat3& mat) {
    GLint location = getUniformLocation(name);
    if (location != -1) {
        glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(mat));
    }
}

void Shader::setMat4(const std::string& name, const glm::mat4& mat) {
    GLint location = getUniformLocation(name);
    if (location != -1) {
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat));
    }
}

// 批量设置uniform方法
void Shader::setUniforms(const std::unordered_map<std::string, int>& intUniforms) {
    for (const auto& [name, value] : intUniforms) {
        setInt(name, value);
    }
}

void Shader::setUniforms(const std::unordered_map<std::string, float>& floatUniforms) {
    for (const auto& [name, value] : floatUniforms) {
        setFloat(name, value);
    }
}

void Shader::setUniforms(const std::unordered_map<std::string, glm::vec3>& vec3Uniforms) {
    for (const auto& [name, value] : vec3Uniforms) {
        setVec3(name, value);
    }
}

void Shader::setUniforms(const std::unordered_map<std::string, glm::mat4>& mat4Uniforms) {
    for (const auto& [name, value] : mat4Uniforms) {
        setMat4(name, value);
    }
}

// 工具方法
void Shader::printActiveUniforms() const {
    if (!isValid()) {
        std::cerr << "ERROR::SHADER::PROGRAM_NOT_VALID" << std::endl;
        return;
    }
    
    GLint numUniforms;
    glGetProgramiv(programID, GL_ACTIVE_UNIFORMS, &numUniforms);
    
    std::cout << "Active Uniforms (" << numUniforms << "):" << std::endl;
    for (int i = 0; i < numUniforms; ++i) {
        char name[256];
        GLsizei length;
        GLint size;
        GLenum type;
        glGetActiveUniform(programID, i, sizeof(name), &length, &size, &type, name);
        std::cout << "  " << i << ": " << name << " (type: " << type << ", size: " << size << ")" << std::endl;
    }
}

bool Shader::hasUniform(const std::string& name) const {
    if (!isValid()) return false;
    return glGetUniformLocation(programID, name.c_str()) != -1;
}

void Shader::reload() {
    if (vertexPath.empty() || fragmentPath.empty()) {
        std::cerr << "ERROR::SHADER::NO_PATHS_TO_RELOAD" << std::endl;
        return;
    }
    
    if (programID != 0) {
        glDeleteProgram(programID);
        programID = 0;
        compiled = false;
        uniformCache.clear();
    }
    
    // 重新构造
    *this = Shader(vertexPath, fragmentPath);
} 