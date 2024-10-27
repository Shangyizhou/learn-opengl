#include "shader_proxy.h"

void checkShader(const std::string& str) {
    std::cout << "Vertex Shader Code:\n" << str << std::endl;
}

ShaderRead::ShaderRead(const std::string vertexPath, const std::string fragmentPath) {
    // 1. 从文件路径中获取顶点/片段着色器
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;

    // 保证ifstream对象可以抛出异常：
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try {
        // 打开文件
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;
        // 读取文件的缓冲内容到数据流中
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();       
        // 关闭文件处理器
        vShaderFile.close();
        fShaderFile.close();
        // 转换数据流到string
        vertexProgramString   = vShaderStream.str();
        fragmentProgramString = fShaderStream.str();     

        checkShader(vertexProgramString);
        checkShader(fragmentProgramString);
    }
    catch(std::ifstream::failure e) {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
        return;
    }
}


ShaderProxy::ShaderProxy(const std::string& vertexPath, const std::string& fragmentPath) {
    reader = std::make_unique<ShaderRead>(vertexPath, fragmentPath);
}

ShaderProxy::~ShaderProxy() {
    
}

ShaderProxy::ErrorType ShaderProxy::compile() {
    const char* vertexShaderSource = nullptr;
    const char* fragmentShaderSource = nullptr;
    if (reader) {
        vertexShaderSource = reader->getVertexProgram().c_str();
        fragmentShaderSource = reader->getVertexProgram().c_str();
    }

    vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aColor;

    out vec3 ourColor;

    void main()
    {
        gl_Position = vec4(aPos, 1.0);
        ourColor = aColor;
    })";

    fragmentShaderSource = R"(
    #version 330 core
    out vec4 FragColor;

    in vec3 ourColor;

    void main()
    {
        FragColor = vec4(ourColor, 1.0f);
    }
    )"; 


    // vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        return ErrorType::COMPILE_VERTEX_SHADER_ERROR;
    }

    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
        return ErrorType::COMPILE_FRAGMENT_SHADER_ERROR;
    }

    // link shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        return ErrorType::COMPILE_PROGRAM_SUCCESS;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    shader_program = shaderProgram;
    checkCompileErrors(shader_program, "PROGRAM");


    std::cout << "SUCCESS::SHADER::PROGRAM::LINK\n" << infoLog << std::endl;
    return ErrorType::COMPILE_PROGRAM_SUCCESS;
}

void ShaderProxy::release() {
    glDeleteProgram(shader_program);
}


