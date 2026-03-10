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

    void ShaderProgram::AttachShader(const ShaderModule& shader) {
        glAttachShader(m_RendererID, shader.GetID());
        m_AttachedShaders.push_back(shader.GetID());
    }

    void ShaderProgram::Link() {
        glLinkProgram(m_RendererID);

        int success;
        glGetProgramiv(m_RendererID, GL_LINK_STATUS, &success);

        // FIX: встановлюємо прапорець і не використовуємо зламану програму
        if (!success) {
            char infoLog[1024];
            glGetProgramInfoLog(m_RendererID, sizeof(infoLog), nullptr, infoLog);
            CW_ERROR_LOG("ShaderProgram linking failed:\n{0}", infoLog);
            m_IsLinked = false;
        } else {
            m_IsLinked = true;
        }

        for (auto id : m_AttachedShaders) {
            glDetachShader(m_RendererID, id);
        }
        m_AttachedShaders.clear();
    }

    void ShaderProgram::Bind() const {
        // FIX: не біндимо зламану програму
        if (!m_IsLinked) {
            CW_WARN_LOG("Attempted to Bind a shader program that failed to link!");
            return;
        }
        glUseProgram(m_RendererID);
    }

    void ShaderProgram::Unbind() const {
        glUseProgram(0);
    }

    // ---------------------------------------------------------------------------
    // Uniform Setters
    // ---------------------------------------------------------------------------

    void ShaderProgram::SetInt(const std::string& name, int value) {
        glUniform1i(GetUniformLocation(name), value);
    }

    void ShaderProgram::SetInt2(const std::string& name, int v1, int v2) {
        glUniform2i(GetUniformLocation(name), v1, v2);
    }

    void ShaderProgram::SetInt3(const std::string& name, int v1, int v2, int v3) {
        glUniform3i(GetUniformLocation(name), v1, v2, v3);
    }

    void ShaderProgram::SetInt4(const std::string& name, int v1, int v2, int v3, int v4) {
        glUniform4i(GetUniformLocation(name), v1, v2, v3, v4);
    }

    void ShaderProgram::SetIntArray(const std::string& name, int* values, uint32_t count) {
        glUniform1iv(GetUniformLocation(name), count, values);
    }

    void ShaderProgram::SetFloat(const std::string& name, float value) {
        glUniform1f(GetUniformLocation(name), value);
    }

    void ShaderProgram::SetFloat2(const std::string& name, float v1, float v2) {
        glUniform2f(GetUniformLocation(name), v1, v2);
    }

    void ShaderProgram::SetFloat3(const std::string& name, float v1, float v2, float v3) {
        glUniform3f(GetUniformLocation(name), v1, v2, v3);
    }

    void ShaderProgram::SetFloat4(const std::string& name, float v1, float v2, float v3, float v4) {
        glUniform4f(GetUniformLocation(name), v1, v2, v3, v4);
    }

    void ShaderProgram::SetDouble2(const std::string& name, double v1, double v2) {
        glUniform2d(GetUniformLocation(name), v1, v2);
    }

    void ShaderProgram::SetDouble3(const std::string& name, double v1, double v2, double v3) {
        glUniform3d(GetUniformLocation(name), v1, v2, v3);
    }

    void ShaderProgram::SetDouble4(const std::string& name, double v1, double v2, double v3, double v4) {
        glUniform4d(GetUniformLocation(name), v1, v2, v3, v4);
    }

    void ShaderProgram::SetMat4(const std::string& name, const glm::mat4& matrix) {
        glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(matrix));
    }

    int ShaderProgram::GetUniformLocation(const std::string& name) {
        auto it = m_UniformLocationCache.find(name);
        if (it != m_UniformLocationCache.end())
            return it->second;

        int location = glGetUniformLocation(m_RendererID, name.c_str());
        if (location == -1) {
            CW_WARN_LOG("Uniform '{0}' not found in shader program!", name);
        }
        m_UniformLocationCache[name] = location;
        return location;
    }
}