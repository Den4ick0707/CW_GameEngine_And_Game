#include "shader_module.h"
#include <glad/glad.h>
#include <fstream>
#include <sstream>
#include <iostream>

namespace Engine::Graphics {

    ShaderModule::ShaderModule(const std::filesystem::path& source, ShaderType type)
        : m_Type(type), m_ID(0)
    {
        // 1. Спочатку читаємо файл. Якщо файлу немає - немає сенсу створювати шейдер.
        std::string sourceCode = ReadFromFile(source);
        if (sourceCode.empty()) {
           // CW_ERROR_LOG("Shader source is empty or file not found: {0}", source.string());
            return;
        }

        // 2. Визначаємо тип OpenGL
        GLenum shader_type_gl;
        std::string shader_type_str;

        switch (type) {
            case ShaderType::VERTEX:
                shader_type_gl = GL_VERTEX_SHADER;
                shader_type_str = "VERTEX";
                break;
            case ShaderType::FRAGMENT:
                shader_type_gl = GL_FRAGMENT_SHADER;
                shader_type_str = "FRAGMENT";
                break;
            case ShaderType::GEOMETRY:
                shader_type_gl = GL_GEOMETRY_SHADER;
                shader_type_str = "GEOMETRY";
                break;
            case ShaderType::TESSELLATION_CONTROL:
                shader_type_gl = GL_TESS_CONTROL_SHADER;
                shader_type_str = "TESS_CONTROL";
                break;
            case ShaderType::TESSELLATION_EVALUATION:
                shader_type_gl = GL_TESS_EVALUATION_SHADER;
                shader_type_str = "TESS_EVAL";
                break;
            case ShaderType::COMPUTE:
                shader_type_gl = GL_COMPUTE_SHADER;
                shader_type_str = "COMPUTE";
                break;
            default:
               // CW_ERROR_LOG("Unknown Shader Type!");
                return;
        }

        // 3. Створюємо та компілюємо
        m_ID = glCreateShader(shader_type_gl);
        const char* shader_str = sourceCode.c_str();
        glShaderSource(m_ID, 1, &shader_str, NULL);
        glCompileShader(m_ID);

        CheckCompileErrors(m_ID, shader_type_str);
    }

    ShaderModule::~ShaderModule() {
        if (m_ID != 0) glDeleteShader(m_ID);
    }

    // Move Constructor implementation example
    ShaderModule::ShaderModule(ShaderModule&& other) noexcept
        : m_ID(other.m_ID), m_Type(other.m_Type)
    {
        other.m_ID = 0; // Забираємо володіння, старий об'єкт стає пустим
    }

    ShaderModule& ShaderModule::operator=(ShaderModule&& other) noexcept {
        if (this != &other) {
            if (m_ID != 0) glDeleteShader(m_ID); // Видаляємо старий ресурс
            m_ID = other.m_ID;
            m_Type = other.m_Type;
            other.m_ID = 0;
        }
        return *this;
    }

    std::string ShaderModule::ReadFromFile(const std::filesystem::path& path) {
        std::string content;
        std::ifstream file;

        // Прибираємо failbit. У ігрових рушіях краще перевіряти if(!file.is_open()),
        // ніж ловити ексепшини, які можуть поводитись по-різному на різних платформах.
        // Але твій підхід теж валідний, якщо тобі так зручніше.
        file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

        try {
            file.open(path);
            std::stringstream fileStream;
            fileStream << file.rdbuf();
            file.close();
            content = fileStream.str();
        } catch (const std::ifstream::failure& e) {
            // Використовуємо твій логер або cerr
            std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << path << std::endl;
            return "";
        }
        return content;
    }

    void ShaderModule::CheckCompileErrors(uint32_t shader, const std::string& type) {
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