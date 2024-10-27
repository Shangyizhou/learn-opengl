#include <iostream>
#include <memory>
#include <fstream>
#include <sstream>
#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class ShaderRead {
public:
    ShaderRead(const std::string vertexPath, const std::string fragmentPath);

    std::string getVertexProgram() const { return vertexProgramString; }
    std::string getFragmentProgram() const { return fragmentProgramString; }

private:
    std::string vertexProgramString = "";
    std::string fragmentProgramString = "";
};

// Shader代理器
class ShaderProxy {
private:
    enum class ErrorType {
        COMPILE_PROGRAM_SUCCESS = 0,
        COMPILE_VERTEX_SHADER_ERROR,
        COMPILE_FRAGMENT_SHADER_ERROR,
        LINK_ERROR,
    };

public:
    ShaderProxy(const std::string& vertexPath, const std::string& fragmentPath);

    ~ShaderProxy();

    ErrorType compile();

    void release();

    void use() 
    { 
        glUseProgram(shader_program); 
    }
    // utility uniform functions
    // ------------------------------------------------------------------------
    void setBool(const std::string &name, bool value) const
    {         
        glUniform1i(glGetUniformLocation(shader_program, name.c_str()), (int)value); 
    }
    // ------------------------------------------------------------------------
    void setInt(const std::string &name, int value) const
    { 
        glUniform1i(glGetUniformLocation(shader_program, name.c_str()), value); 
    }
    // ------------------------------------------------------------------------
    void setFloat(const std::string &name, float value) const
    { 
        glUniform1f(glGetUniformLocation(shader_program, name.c_str()), value); 
    }
    
private:


private:
    // utility function for checking shader compilation/linking errors.
    // ------------------------------------------------------------------------
    void checkCompileErrors(unsigned int shader, std::string type)
    {
        int success;
        char infoLog[1024];
        if (type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
    }
    
    std::unique_ptr<ShaderRead> reader;
    unsigned int shader_program;
};
