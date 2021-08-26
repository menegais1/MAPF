//
// Created by menegais1 on 01/08/2020.
//

#include "Shader.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <iostream>


std::string Shader::loadShaderFile(std::string filePath) {
    std::string sourceCode;

    std::fstream file(filePath, std::ios::in);
    if (!file.is_open()) {
        std::cerr << "ERROR OPENING FILE " << filePath << std::endl;
        return nullptr;
    }

    while (!file.eof()) {
        sourceCode.append(1, file.get());
    }
    sourceCode[sourceCode.length() - 1] = 0;
    return sourceCode;
}

unsigned int Shader::createVertexShader(std::string shader) {
    std::string sourceCode = loadShaderFile(shader);
    const char *source = sourceCode.c_str();
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &source, nullptr);
    glCompileShader(vertexShader);

    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" <<
                  infoLog << std::endl;
        return -1;
    }

    return vertexShader;
}

unsigned int Shader::createFragmentShader(std::string shader) {
    std::string sourceCode = loadShaderFile(shader);
    const char *source = sourceCode.c_str();
    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &source, nullptr);
    glCompileShader(fragmentShader);

    int success;
    char infoLog[512];
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" <<
                  infoLog << std::endl;
        return -1;
    }

    return fragmentShader;
}

Shader::Shader(unsigned int vertexShader, unsigned int fragmentShader) {
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    int success;
    GLchar infoLog[1024];
    glGetProgramiv(shaderProgram, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 1024, NULL, infoLog);
        std::cerr << "ERROR::LINKING_SHADER::\n" <<
                  infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void Shader::setUniform(int location, int value) {
    glUniform1i(location, value);
}

void Shader::setUniform(int location, float value) {
    glUniform1f(location, value);
}

void Shader::setUniform(int location, glm::vec2 value) {
    glUniform2f(location, value.x, value.y);
}

void Shader::setUniform(int location, glm::vec3 value) {
    glUniform3f(location, value.x, value.y, value.z);
}

void Shader::setUniform(int location, glm::vec4 value) {
    glUniform4f(location, value.x, value.y, value.z, value.w);
}

void Shader::setUniform(int location, glm::mat4x4 value) {
    //This cast to float is needed for some reason
    float values[16];
    int cont = 0;
    for (int l = 0; l < 4; ++l) {
        for (int c = 0; c < 4; ++c) {
            values[cont++] = value[c][l];
        }
    }
    glUniformMatrix4fv(location, 1, true, values);
}

void Shader::setUniform(int location, unsigned int value) {
    glUniform1ui(location, value);
}

void Shader::activateShader() {
    glUseProgram(shaderProgram);
}
