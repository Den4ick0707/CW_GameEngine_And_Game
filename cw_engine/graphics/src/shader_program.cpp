#include "shader_program.h"
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

namespace Engine::Graphics {

    ShaderProgram::ShaderProgram() {
        m_RendererID = glCreateProgram();
    }

    ShaderProgram::~ShaderProgram() {
        glDeleteProgram(m_RendererID);
    }

    void ShaderProgram::AttachShader(const ShaderModule& shader) {
        if (!shader.IsValid()) {
            std::cerr << "[ShaderProgram] Tried to attach invalid shader module.\n";
            return;
        }
        glAttachShader(m_RendererID, shader.GetID());
        m_Attached.push_back(shader.GetID());
    }

    void ShaderProgram::Link() {
        glLinkProgram(m_RendererID);
        CheckLinkErrors();

        // Detach після лінковки — шейдери більше не потрібні програмі
        for (auto id : m_Attached)
            glDetachShader(m_RendererID, id);
        m_Attached.clear();
    }

    void ShaderProgram::CheckLinkErrors() {
        int success = 0;
        glGetProgramiv(m_RendererID, GL_LINK_STATUS, &success);
        m_Linked = (success == GL_TRUE);
        if (!m_Linked) {
            char log[1024];
            glGetProgramInfoLog(m_RendererID, sizeof(log), nullptr, log);
            std::cerr << "[ShaderProgram] Link error:\n" << log << "\n";
        }
    }

    void ShaderProgram::Bind() const {
        if (!m_Linked) {
            std::cerr << "[ShaderProgram] Bind() called on unlinked program!\n";
            return;
        }
        glUseProgram(m_RendererID);
    }

    void ShaderProgram::Unbind() const { glUseProgram(0); }

    int ShaderProgram::GetUniformLocation(const std::string& name) {
        auto it = m_Cache.find(name);
        if (it != m_Cache.end()) return it->second;

        int loc = glGetUniformLocation(m_RendererID, name.c_str());
        if (loc == -1)
            std::cerr << "[ShaderProgram] Uniform not found: '" << name << "'\n";

        m_Cache[name] = loc;
        return loc;
    }

    // ── Setters ──────────────────────────────────────────────────────────────

    void ShaderProgram::SetBool (const std::string& n, bool  v) { glUniform1i (GetUniformLocation(n), v); }
    void ShaderProgram::SetInt  (const std::string& n, int   v) { glUniform1i (GetUniformLocation(n), v); }
    void ShaderProgram::SetFloat(const std::string& n, float v) { glUniform1f (GetUniformLocation(n), v); }

    void ShaderProgram::SetIntArray(const std::string& n, int* v, uint32_t count) {
        glUniform1iv(GetUniformLocation(n), static_cast<GLsizei>(count), v);
    }

    void ShaderProgram::SetFloat2(const std::string& n, float x, float y)
        { glUniform2f(GetUniformLocation(n), x, y); }

    void ShaderProgram::SetFloat3(const std::string& n, float x, float y, float z)
        { glUniform3f(GetUniformLocation(n), x, y, z); }

    void ShaderProgram::SetFloat4(const std::string& n, float x, float y, float z, float w)
        { glUniform4f(GetUniformLocation(n), x, y, z, w); }

    void ShaderProgram::SetVec2(const std::string& n, const glm::vec2& v)
        { glUniform2fv(GetUniformLocation(n), 1, glm::value_ptr(v)); }

    void ShaderProgram::SetVec3(const std::string& n, const glm::vec3& v)
        { glUniform3fv(GetUniformLocation(n), 1, glm::value_ptr(v)); }

    void ShaderProgram::SetVec4(const std::string& n, const glm::vec4& v)
        { glUniform4fv(GetUniformLocation(n), 1, glm::value_ptr(v)); }

    void ShaderProgram::SetMat3(const std::string& n, const glm::mat3& m)
        { glUniformMatrix3fv(GetUniformLocation(n), 1, GL_FALSE, glm::value_ptr(m)); }

    void ShaderProgram::SetMat4(const std::string& n, const glm::mat4& m)
        { glUniformMatrix4fv(GetUniformLocation(n), 1, GL_FALSE, glm::value_ptr(m)); }

} // namespace Engine::Graphics