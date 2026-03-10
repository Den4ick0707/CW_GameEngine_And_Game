#include "shader_program.h"
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#include "../../core/include/logger.h"

namespace Engine::Graphics {
    ShaderProgram::ShaderProgram() {
        m_RendererID = glCreateProgram();
    }

    ShaderProgram::~ShaderProgram() {
        glDeleteProgram(m_RendererID);
    }

    void ShaderProgram::AttachShader(const ShaderModule &shader) {
        glAttachShader(m_RendererID, shader.GetID());
        m_AttachedShaders.push_back(shader.GetID());
    }

    void ShaderProgram::Link() {
        glLinkProgram(m_RendererID);

        int success;
        glGetProgramiv(m_RendererID, GL_LINK_STATUS, &success);
        if (!success) {
            char infoLog[1024];
            glGetProgramInfoLog(m_RendererID, 1024, NULL, infoLog);
            CW_ERROR_LOG(infoLog);
        }

        for (auto id: m_AttachedShaders) {
            glDetachShader(m_RendererID, id);
        }
        m_AttachedShaders.clear();
    }

    void ShaderProgram::Bind() const {
        glUseProgram(m_RendererID);
    }

    void ShaderProgram::Unbind() const {
        glUseProgram(0);
    }

    void ShaderProgram::SetInt(const std::string &name, int value) {
        glUniform1i(GetUniformLocation(name), value);
    }

    void ShaderProgram::SetInt2(const std::string &name, int value_one, int value_two) {
        glUniform2i(GetUniformLocation(name), value_one, value_two);
    }

    void ShaderProgram::SetInt3(const std::string &name, int value_one, int value_two, int value_three) {
        glUniform3i(GetUniformLocation(name), value_one, value_two, value_three);
    }

    void ShaderProgram::SetInt4(const std::string &name, int value_one, int value_two, int value_three,
                                int value_four) {
        glUniform4i(GetUniformLocation(name), value_one, value_two, value_three, value_four);
    }

    void ShaderProgram::SetIntArray(const std::string &name, int *values, uint32_t count) {
        glUniform1iv(GetUniformLocation(name), count, values);
    }

    void ShaderProgram::SetFloat(const std::string &name, float value) {
        glUniform1f(GetUniformLocation(name), value);
    }

    void ShaderProgram::SetFloat2(const std::string &name, float value_one, float value_two) {
        glUniform2f(GetUniformLocation(name), value_one, value_two);
    }

    void ShaderProgram::SetFloat3(const std::string &name, float value_one, float value_two, float value_three) {
        glUniform3f(GetUniformLocation(name), value_one, value_two, value_three);
    }

    void ShaderProgram::SetFloat4(const std::string &name, float value_one, float value_two, float value_three,
                                  float value_four) {
        glUniform4f(GetUniformLocation(name), value_one, value_two, value_three, value_four);
    }

    void ShaderProgram::SetDouble2(const std::string &name, double value_one, double value_two) {
        glUniform2d(GetUniformLocation(name), value_one, value_two);
    }

    void ShaderProgram::SetDouble3(const std::string &name, double value_one, double value_two, double value_three) {
        glUniform3d(GetUniformLocation(name), value_one, value_two, value_three);
    }

    void ShaderProgram::SetDouble4(const std::string &name, double value_one, double value_two, double value_three,
                                   double value_four) {
        glUniform4d(GetUniformLocation(name), value_one, value_two, value_three, value_four);
    }

    void ShaderProgram::SetMat4(const std::string &name, const glm::mat4 &matrix) {
        // GL_FALSE означає, що матрицю не потрібно транспонувати
        glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(matrix));
    }

    int ShaderProgram::GetUniformLocation(const std::string &name) {
        if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end()) {
            return m_UniformLocationCache[name];
        }

        int location = glGetUniformLocation(m_RendererID, name.c_str());

        m_UniformLocationCache[name] = location;
        return location;
    }
}
