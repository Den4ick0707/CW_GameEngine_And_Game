#ifndef OPENGLPROJECT_SHADERMODULE_HPP
#define OPENGLPROJECT_SHADERMODULE_HPP
#include "pch.h"

namespace Graphics {
    enum class ShaderType {
        VERTEX,
        FRAGMENT,
        GEOMETRY,
        TESSELATION,
        COMPUTE
    };

    class ShaderModule {
    public:
        ShaderModule(const std::filesystem::path& source, ShaderType shader_type);
        ShaderModule(const ShaderModule& other)=delete;
        ShaderModule& operator=(const ShaderModule& other)=delete;
        ~ShaderModule();

        [[nodiscard]] ShaderType GetType() const { return m_Type; }
        [[nodiscard]] GLuint GetID() const { return m_ID; }

    private:
        GLuint m_ID = 0;
        ShaderType m_Type;

        std::string ReadFromFile(const std::filesystem::path &path);
        void CheckCompileErrors(GLuint shader, const std::string &type);
    };
}
#endif