#include "pch.h"
#include "ShaderModule.h"
#include <glad/glad.h>
#include <fstream>
#include <sstream>

namespace Engine::Graphics {

    ShaderModule::ShaderModule(const std::filesystem::path& source, ShaderType type)
        : m_Type(type), m_ID(0)
    {
        GLenum shader_type_gl;
        std::string shader_type_str;

        switch (type) {
            case ShaderType::VERTEX:      shader_type_gl = GL_VERTEX_SHADER; shader_type_str = "VERTEX"; break;
            case ShaderType::FRAGMENT:    shader_type_gl = GL_FRAGMENT_SHADER; shader_type_str = "FRAGMENT"; break;
            case ShaderType::GEOMETRY:    shader_type_gl = GL_GEOMETRY_SHADER; shader_type_str = "GEOMETRY"; break;
            case ShaderType::COMPUTE:     shader_type_gl = GL_COMPUTE_SHADER; shader_type_str = "COMPUTE"; break;
            default: std::cerr << "ERROR::SHADER::UNKNOWN_TYPE" << std::endl; return;
        }

        m_ID = glCreateShader(shader_type_gl);

        std::string sourceCode = ReadFromFile(source);
        if (sourceCode.empty()) return;

        const char* shader_str = sourceCode.c_str();
        glShaderSource(m_ID, 1, &shader_str, NULL);
        glCompileShader(m_ID);

        CheckCompileErrors(m_ID, shader_type_str);
    }

    ShaderModule::~ShaderModule() {
        if (m_ID != 0) glDeleteShader(m_ID);
    }

    std::string ShaderModule::ReadFromFile(const std::filesystem::path& path) {
        std::string content;
        std::ifstream file;

        // Забезпечуємо, що ifstream може кидати виключення
        file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

        try {
            file.open(path);
            std::stringstream fileStream;
            fileStream << file.rdbuf();
            file.close();
            content = fileStream.str();
        } catch (std::ifstream::failure& e) {
            std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << path << std::endl;
            std::cerr << "Exception details: " << e.what() << std::endl;
            return "";
        }
        return content;
    }

    void ShaderModule::CheckCompileErrors(GLuint shader, const std::string& type) {
        int success;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            GLchar infoLog[1024];
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cerr << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n"
                      << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
}