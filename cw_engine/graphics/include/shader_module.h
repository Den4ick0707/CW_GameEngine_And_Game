#ifndef CW_ENGINE_GRAPHICS_SHADER_MODULE_H
#define CW_ENGINE_GRAPHICS_SHADER_MODULE_H

#include <string>
#include <filesystem>
#include <cstdint> // Важливо для uint32_t

namespace Engine::Graphics {

    /// @brief Represents the type of the shader stage.
    enum class ShaderType {
        VERTEX,
        FRAGMENT,
        GEOMETRY,
        TESSELLATION_CONTROL,    // Уточнення для Tessellation
        TESSELLATION_EVALUATION, // Уточнення для Tessellation
        COMPUTE
    };

    /// @brief Represents a single compiled shader object (e.g., Vertex Shader).
    /// @details
    /// This class handles loading source code from disk, compiling it,
    /// and checking for errors. It does NOT link the program.
    /// Used as a building block for ShaderProgram.
    class ShaderModule {
    public:
        /// @brief Loads and compiles a shader from a file.
        /// @param source Path to the shader file (.vert, .frag, etc.).
        /// @param shader_type The stage of the pipeline (Vertex, Fragment...).
        ShaderModule(const std::filesystem::path &source, ShaderType shader_type);

        /// @brief Destroys the shader object (glDeleteShader).
        ~ShaderModule();

        // Prevent copying (ShaderModule owns the GL ID)
        ShaderModule(const ShaderModule &other) = delete;
        ShaderModule &operator=(const ShaderModule &other) = delete;

        // Allow moving (optional, but good practice)
        ShaderModule(ShaderModule&& other) noexcept;
        ShaderModule& operator=(ShaderModule&& other) noexcept;

        /// @brief Gets the shader type.
        [[nodiscard]] ShaderType GetType() const { return m_Type; }

        /// @brief Gets the OpenGL ID of the compiled shader.
        [[nodiscard]] uint32_t GetID() const { return m_ID; }

    private:
        /// @brief Reads file content into a string.
        std::string ReadFromFile(const std::filesystem::path &path);

        /// @brief Checks OpenGL compilation status.
        void CheckCompileErrors(uint32_t shader, const std::string &type);

    private:
        uint32_t m_ID = 0;
        ShaderType m_Type;
    };
}

#endif // CW_ENGINE_GRAPHICS_SHADER_MODULE_H