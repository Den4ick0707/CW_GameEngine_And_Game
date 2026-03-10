#pragma once

#include "shader_module.h"
#include <glm/glm.hpp>
#include <unordered_map>
#include <string>
#include <vector>
#include <cstdint>

namespace Engine::Graphics {

    /// @brief Manages the full OpenGL shader program lifecycle.
    class ShaderProgram {
    public:
        ShaderProgram();
        ~ShaderProgram();

        void AttachShader(const ShaderModule& shader);

        /// @brief Links all attached shaders. Sets m_IsLinked = false on failure.
        void Link();

        /// @brief Binds this shader program. No-op if linking failed.
        void Bind() const;

        void Unbind() const;

        /// @brief Returns true if the program was linked successfully.
        [[nodiscard]] bool IsLinked() const { return m_IsLinked; }

        // --- Uniform Setters ---
        // NOTE: Shader must be bound before calling these!

        void SetInt      (const std::string& name, int value);
        void SetInt2     (const std::string& name, int v1, int v2);
        void SetInt3     (const std::string& name, int v1, int v2, int v3);
        void SetInt4     (const std::string& name, int v1, int v2, int v3, int v4);
        void SetIntArray (const std::string& name, int* values, uint32_t count);

        void SetFloat    (const std::string& name, float value);
        void SetFloat2   (const std::string& name, float v1, float v2);
        void SetFloat3   (const std::string& name, float v1, float v2, float v3);
        void SetFloat4   (const std::string& name, float v1, float v2, float v3, float v4);

        void SetDouble2  (const std::string& name, double v1, double v2);
        void SetDouble3  (const std::string& name, double v1, double v2, double v3);
        void SetDouble4  (const std::string& name, double v1, double v2, double v3, double v4);

        void SetMat4     (const std::string& name, const glm::mat4& matrix);

    private:
        int GetUniformLocation(const std::string& name);

    private:
        uint32_t m_RendererID;
        bool     m_IsLinked = false; // FIX: відстежуємо успішність лінковки

        std::vector<uint32_t>              m_AttachedShaders;
        std::unordered_map<std::string, int> m_UniformLocationCache;
    };
}