#include "pch.h"
#include "ShaderProgram.h"
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

namespace Engine::Graphics {

    ShaderProgram::ShaderProgram() {
        m_RendererID = glCreateProgram();
    }

    ShaderProgram::~ShaderProgram() {
        glDeleteProgram(m_RendererID);
    }

    void ShaderProgram::AttachShader(const ShaderModule& shader) {
        glAttachShader(m_RendererID, shader.GetID());
        m_AttachedShaders.push_back(shader.GetID());
    }

    void ShaderProgram::Link() {
        glLinkProgram(m_RendererID);

        int success;
        glGetProgramiv(m_RendererID, GL_LINK_STATUS, &success);
        if (!success) {
            char infoLog[1024];
            glGetProgramInfoLog(m_RendererID, 512, NULL, infoLog);
            std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        }

        // Після лінковки шейдери можна від'єднати
        for (auto id : m_AttachedShaders) {
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

    // --- Uniforms ---

    void ShaderProgram::SetInt(const std::string& name, int value) {
        glUniform1i(GetUniformLocation(name), value);
    }

    void ShaderProgram::SetIntArray(const std::string& name, int* values, uint32_t count) {
        glUniform1iv(GetUniformLocation(name), count, values);
    }

    void ShaderProgram::SetFloat(const std::string& name, float value) {
        glUniform1f(GetUniformLocation(name), value);
    }

    void ShaderProgram::SetFloat3(const std::string& name, const glm::vec3& value) {
        glUniform3f(GetUniformLocation(name), value.x, value.y, value.z);
    }

    void ShaderProgram::SetFloat4(const std::string& name, const glm::vec4& value) {
        glUniform4f(GetUniformLocation(name), value.x, value.y, value.z, value.w);
    }

    void ShaderProgram::SetMat4(const std::string& name, const glm::mat4& value) {
        // GL_FALSE означає, що ми не транспонуємо матрицю (GLM вже підходить для OpenGL)
        glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
    }

    int ShaderProgram::GetUniformLocation(const std::string& name) {
        if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
            return m_UniformLocationCache[name];

        int location = glGetUniformLocation(m_RendererID, name.c_str());
        if (location == -1) {
            // Можна розкоментувати для дебагу, але часто змінні оптимізуються драйвером і це ок
            // std::cout << "Warning: uniform '" << name << "' doesn't exist!" << std::endl;
        }

        m_UniformLocationCache[name] = location;
        return location;
    }
}