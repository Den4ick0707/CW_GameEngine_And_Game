#ifndef OPENGLPROJECT_SHADERPROGRAM_HPP
#define OPENGLPROJECT_SHADERPROGRAM_HPP
#include "pch.h"
#include "ShaderModule.h"
namespace Graphics {
    class ShaderProgram {
        GLuint m_RendererID;
        std::vector<unsigned int> m_AttachedShaders;
        std::unordered_map<std::string, int> m_UniformLocationCache;

        int GetUniformLocation(const std::string &name);

    public:
        ShaderProgram();

        ~ShaderProgram();

        //region Shader program methods
        void AttachShader(const ShaderModule &shader);

        void Link();

        void Bind() const;

        void Unbind() const;

        //endregion

        //region Uniforms
        void SetInt(const std::string &name, int value);

        void SetInt2(const std::string &name, const glm::ivec2 &value);

        void SetInt3(const std::string &name, const glm::ivec3 &value);

        void SetInt4(const std::string &name, const glm::ivec4 &value);

        void SetFloat(const std::string &name, float value);

        void SetFloat2(const std::string &name, const glm::vec2 &value);

        void SetFloat3(const std::string &name, const glm::vec3 &value);

        void SetFloat4(const std::string &name, const glm::vec4 &value);

        void SetMat3(const std::string &name, const glm::mat3 &value);

        void SetMat4(const std::string &name, const glm::mat4 &value);

        void SetBool(const std::string &name, bool value);

        void SetInt2(const std::string &name, const int v0, const int v1);

        void SetInt3(const std::string &name, const int v0, const int v1, const int v2);

        void SetInt4(const std::string &name, const int v0, const int v1, const int v2, const int v3);

        void SetFloat2(const std::string &name, const float v0, const float v1);

        void SetFloat3(const std::string &name, const float v0, const float v1, const float  v2);

        void SetFloat4(const std::string &name, const float v0, const float v1, const float v2, const float v3);


        //endregion
    };
}
#endif //OPENGLPROJECT_SHADERPROGRAM_HPP
