#pragma once

#include "shader_module.h"
#include <glm/glm.hpp>
#include <unordered_map>
#include <string>
#include <vector>
#include <cstdint>

namespace Engine::Graphics {

    /// @brief Зв'язана шейдерна програма (Vertex + Fragment + ...).
    /// @details
    /// Кешує uniform location-и для швидкого доступу.
    /// Не викликає uniform setters якщо програма не зв'язана.
    class ShaderProgram {
    public:
        ShaderProgram();
        ~ShaderProgram();

        ShaderProgram(const ShaderProgram&)            = delete;
        ShaderProgram& operator=(const ShaderProgram&) = delete;

        /// @brief Прикріпити скомпільований ShaderModule.
        void AttachShader(const ShaderModule& shader);

        /// @brief Зв'язати всі прикріплені модулі. Перевіряє помилки.
        void Link();

        void Bind()   const;
        void Unbind() const;

        [[nodiscard]] bool     IsLinked()    const { return m_Linked; }
        [[nodiscard]] uint32_t GetRendererID() const { return m_RendererID; }

        // ── Uniform Setters ───────────────────────────────────────────────
        // Програма має бути активна (Bind()) перед викликом!

        void SetBool    (const std::string& name, bool  value);
        void SetInt     (const std::string& name, int   value);
        void SetIntArray(const std::string& name, int* values, uint32_t count);
        void SetFloat   (const std::string& name, float value);
        void SetFloat2  (const std::string& name, float x, float y);
        void SetFloat3  (const std::string& name, float x, float y, float z);
        void SetFloat4  (const std::string& name, float x, float y, float z, float w);
        void SetVec2    (const std::string& name, const glm::vec2& v);
        void SetVec3    (const std::string& name, const glm::vec3& v);
        void SetVec4    (const std::string& name, const glm::vec4& v);
        void SetMat3    (const std::string& name, const glm::mat3& m);
        void SetMat4    (const std::string& name, const glm::mat4& m);

    private:
        int  GetUniformLocation(const std::string& name);
        void CheckLinkErrors();

        uint32_t m_RendererID = 0;
        bool     m_Linked     = false;

        std::vector<uint32_t>                m_Attached;
        std::unordered_map<std::string, int> m_Cache;   // uniform location cache
    };

} // namespace Engine::Graphics