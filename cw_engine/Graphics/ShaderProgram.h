#pragma once
#include "pch.h"
#include "ShaderModule.h"
#include <glm/glm.hpp>
#include <unordered_map>

namespace Engine::Graphics {

    class ShaderProgram {
    public:
        ShaderProgram();
        ~ShaderProgram();

        void AttachShader(const ShaderModule& shader);
        void Link();

        void Bind() const;
        void Unbind() const;

        // Uniforms
        void SetInt(const std::string& name, int value);
        void SetIntArray(const std::string& name, int* values, uint32_t count);
        void SetFloat(const std::string& name, float value);
        void SetFloat3(const std::string& name, const glm::vec3& value);
        void SetFloat4(const std::string& name, const glm::vec4& value);
        void SetMat4(const std::string& name, const glm::mat4& value);

    private:
        int GetUniformLocation(const std::string& name);

    private:
        uint32_t m_RendererID;
        std::vector<uint32_t> m_AttachedShaders;
        std::unordered_map<std::string, int> m_UniformLocationCache;
    };
}