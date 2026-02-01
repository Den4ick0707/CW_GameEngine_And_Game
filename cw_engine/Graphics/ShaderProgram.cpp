#include "ShaderProgram.h"
namespace Graphics {
    //region Constructor & Destructor
    ShaderProgram::ShaderProgram() {
        m_RendererID = glCreateProgram();
    }

    ShaderProgram::~ShaderProgram() {
        glDeleteProgram(m_RendererID);
        m_RendererID = 0;
    }

    //endregion

    //region Shader program methods
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
            glGetProgramInfoLog(m_RendererID, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
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

    int ShaderProgram::GetUniformLocation(const std::string &name) {
        if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end()) {
            return m_UniformLocationCache[name];
        }

        int location = glGetUniformLocation(m_RendererID, name.c_str());

        if (location == -1) {
            std::cout << "Warning: uniform '" << name << "' doesn't exist!" << std::endl;
        }

        m_UniformLocationCache[name] = location;
        return location;
    }

    //endregion

    //region Uniforms
    void ShaderProgram::SetInt(const std::string &name, int value) {
        glUniform1i(GetUniformLocation(name), value);
    }

    void ShaderProgram::SetInt2(const std::string &name, const glm::ivec2 &value) {
        glUniform2i(GetUniformLocation(name), value.x, value.y);
    }

    void ShaderProgram::SetInt3(const std::string &name, const glm::ivec3 &value) {
        glUniform3i(GetUniformLocation(name), value.x, value.y, value.z);
    }

    void ShaderProgram::SetInt4(const std::string &name, const glm::ivec4 &value) {
        glUniform4i(GetUniformLocation(name), value.r, value.g, value.b, value.a);
    }

    void ShaderProgram::SetFloat(const std::string &name, float value) {
        glUniform1f(GetUniformLocation(name), value);
    }

    void ShaderProgram::SetFloat2(const std::string &name, const glm::vec2 &value) {
        glUniform2f(GetUniformLocation(name), value.x, value.y);
    }

    void ShaderProgram::SetFloat3(const std::string &name, const glm::vec3 &value) {
        glUniform3f(GetUniformLocation(name), value.x, value.y, value.z);
    }

    void ShaderProgram::SetFloat4(const std::string &name, const glm::vec4 &value) {
        glUniform4f(GetUniformLocation(name), value.r, value.g, value.b, value.a);
    }

    void ShaderProgram::SetMat3(const std::string &name, const glm::mat3 &value) {
        glUniformMatrix3fv(GetUniformLocation(name), 1,GL_FALSE, glm::value_ptr(value));
    }

    void ShaderProgram::SetMat4(const std::string &name, const glm::mat4 &value) {
        glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
    }

    void ShaderProgram::SetInt2(const std::string &name, const int v0, const int v1) {
        glUniform2i(GetUniformLocation(name), v0, v1);
    }

    void ShaderProgram::SetInt3(const std::string &name, const int v0, const int v1, const int v2) {
        glUniform3i(GetUniformLocation(name), v0, v1, v2);
    }

    void ShaderProgram::SetInt4(const std::string &name, const int v0, const int v1, const int v2, const int v3) {
        glUniform4i(GetUniformLocation(name), v0, v1, v2, v3);
    }

    void ShaderProgram::SetFloat2(const std::string &name, const float v0, const float v1) {
        glUniform2f(GetUniformLocation(name), v0, v1);
    }

    void ShaderProgram::SetFloat3(const std::string &name, const float v0, const float v1, const float v2) {
        glUniform3f(GetUniformLocation(name), v0, v1, v2);
    }

    void ShaderProgram::SetFloat4(const std::string &name, const float v0, const float v1, const float v2, const float v3) {
        glUniform4f(GetUniformLocation(name), v0, v1, v2, v3);
    }

    //endregion
}