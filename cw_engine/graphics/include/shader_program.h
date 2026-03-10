#ifndef CW_DARYEV_SHADER_PROGRAM_H
#define CW_DARYEV_SHADER_PROGRAM_H

#include "shader_module.h"
#include <glm/glm.hpp>
#include <unordered_map>
#include <string>
#include <vector>
#include <cstdint>

namespace Engine::Graphics {

    /// @brief Manages the full OpenGL shader program lifecycle.
    /// @details
    /// A Shader Program is the final linked version of multiple shader stages
    /// (e.g., Vertex Shader + Fragment Shader).
    ///
    /// Usage flow:
    /// 1. Create ShaderProgram.
    /// 2. Create ShaderModules (Vertex, Fragment).
    /// 3. AttachShader() for each module.
    /// 4. Link().
    /// 5. Bind() and use SetFloat/SetInt to upload uniforms.
    class ShaderProgram {
    public:
        /// @brief Creates an empty shader program on the GPU (glCreateProgram).
        ShaderProgram();

        /// @brief Destroys the shader program (glDeleteProgram).
        ~ShaderProgram();

        /// @brief Attaches a compiled shader module to this program.
        /// @param shader The compiled shader module (Vertex, Fragment, etc.).
        void AttachShader(const ShaderModule &shader);

        /// @brief Links all attached shaders into the final executable pipeline.
        /// @details Also performs error checking for the linking process.
        void Link();

        /// @brief Binds this shader program for use in upcoming draw calls.
        void Bind() const;

        /// @brief Unbinds the current shader program (binds 0).
        void Unbind() const;

        // -------------------------------------------------------------------
        // Uniform Setters
        // Note: The shader MUST be bound (Bind()) before setting uniforms!
        // -------------------------------------------------------------------

        /// @name Integer Uniforms
        /// @{
        void SetInt(const std::string &name, int value);
        void SetInt2(const std::string &name, int value_one, int value_two);
        void SetInt3(const std::string &name, int value_one, int value_two, int value_three);
        void SetInt4(const std::string &name, int value_one, int value_two, int value_three, int value_four);

        /// @brief Uploads an array of integers (useful for texture samplers in batch rendering).
        void SetIntArray(const std::string &name, int *values, uint32_t count);
        /// @}

        /// @name Float Uniforms
        /// @{
        void SetFloat(const std::string &name, float value);
        void SetFloat2(const std::string &name, float value_one, float value_two);
        void SetFloat3(const std::string &name, float value_one, float value_two, float value_three);
        void SetFloat4(const std::string &name, float value_one, float value_two, float value_three, float value_four);
        /// @}

        /// @name Double Uniforms (Requires OpenGL 4.0+)
        /// @{
        void SetDouble2(const std::string &name, double value_one, double value_two);
        void SetDouble3(const std::string &name, double value_one, double value_two, double value_three);
        void SetDouble4(const std::string &name, double value_one, double value_two, double value_three, double value_four);
        /// @}

        void SetMat4(const std::string &name, const glm::mat4 &matrix);


    private:
        /// @brief Retrieves the location of a uniform variable.
        /// @details Uses an internal cache (m_UniformLocationCache) to avoid slow glGetUniformLocation calls every frame.
        /// @param name The name of the uniform in the GLSL code.
        /// @return The OpenGL location ID, or -1 if not found.
        int GetUniformLocation(const std::string &name);

    private:
        uint32_t m_RendererID; ///< The OpenGL ID of the linked program.
        std::vector<uint32_t> m_AttachedShaders; ///< Keeps track of attached shaders for cleanup during linking.
        std::unordered_map<std::string, int> m_UniformLocationCache; ///< Caches uniform locations for performance.
    };
}

#endif // CW_DARYEV_SHADER_PROGRAM_H