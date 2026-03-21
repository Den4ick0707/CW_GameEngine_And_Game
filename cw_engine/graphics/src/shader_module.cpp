#include "shader_module.h"
#include <glad/glad.h>
#include <fstream>
#include <sstream>
#include <iostream>

namespace Engine::Graphics {

    static GLenum ToGLShaderType(ShaderType type) {
        switch (type) {
            case ShaderType::Vertex:          return GL_VERTEX_SHADER;
            case ShaderType::Fragment:        return GL_FRAGMENT_SHADER;
            case ShaderType::Geometry:        return GL_GEOMETRY_SHADER;
            case ShaderType::TessControl:     return GL_TESS_CONTROL_SHADER;
            case ShaderType::TessEvaluation:  return GL_TESS_EVALUATION_SHADER;
            case ShaderType::Compute:         return GL_COMPUTE_SHADER;
        }
        return 0;
    }

    static std::string ShaderTypeName(ShaderType type) {
        switch (type) {
            case ShaderType::Vertex:         return "VERTEX";
            case ShaderType::Fragment:       return "FRAGMENT";
            case ShaderType::Geometry:       return "GEOMETRY";
            case ShaderType::TessControl:    return "TESS_CONTROL";
            case ShaderType::TessEvaluation: return "TESS_EVAL";
            case ShaderType::Compute:        return "COMPUTE";
        }
        return "UNKNOWN";
    }

    ShaderModule::ShaderModule(const std::filesystem::path& path, ShaderType type)
        : m_Type(type)
    {
        std::string src = ReadFile(path);
        if (src.empty()) return;

        m_ID = glCreateShader(ToGLShaderType(type));
        const char* cstr = src.c_str();
        glShaderSource(m_ID, 1, &cstr, nullptr);
        glCompileShader(m_ID);
        CheckErrors(ShaderTypeName(type));
    }

    ShaderModule::~ShaderModule() {
        if (m_ID) glDeleteShader(m_ID);
    }

    ShaderModule::ShaderModule(ShaderModule&& o) noexcept
        : m_ID(o.m_ID), m_Type(o.m_Type) { o.m_ID = 0; }

    ShaderModule& ShaderModule::operator=(ShaderModule&& o) noexcept {
        if (this != &o) {
            if (m_ID) glDeleteShader(m_ID);
            m_ID   = o.m_ID;
            m_Type = o.m_Type;
            o.m_ID = 0;
        }
        return *this;
    }

    std::string ShaderModule::ReadFile(const std::filesystem::path& path) {
        std::ifstream file(path);
        if (!file.is_open()) {
            std::cerr << "[ShaderModule] Cannot open: " << path << "\n";
            return {};
        }
        std::ostringstream ss;
        ss << file.rdbuf();
        return ss.str();
    }

    void ShaderModule::CheckErrors(const std::string& typeStr) {
        int success = 0;
        glGetShaderiv(m_ID, GL_COMPILE_STATUS, &success);
        if (!success) {
            char log[1024];
            glGetShaderInfoLog(m_ID, sizeof(log), nullptr, log);
            std::cerr << "[ShaderModule] Compile error (" << typeStr << "):\n"
                      << log << "\n";
            glDeleteShader(m_ID);
            m_ID = 0;
        }
    }

} // namespace Engine::Graphics