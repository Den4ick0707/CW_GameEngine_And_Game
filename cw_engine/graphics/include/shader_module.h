#pragma once

#include <string>
#include <filesystem>
#include <cstdint>

namespace Engine::Graphics {

    enum class ShaderType {
        Vertex,
        Fragment,
        Geometry,
        TessControl,
        TessEvaluation,
        Compute
    };

    /// @brief Один скомпільований шейдерний об'єкт.
    class ShaderModule {
    public:
        ShaderModule(const std::filesystem::path& path, ShaderType type);
        ~ShaderModule();

        ShaderModule(const ShaderModule&)            = delete;
        ShaderModule& operator=(const ShaderModule&) = delete;

        ShaderModule(ShaderModule&& other) noexcept;
        ShaderModule& operator=(ShaderModule&& other) noexcept;

        [[nodiscard]] uint32_t   GetID()   const { return m_ID;   }
        [[nodiscard]] ShaderType GetType() const { return m_Type; }
        [[nodiscard]] bool       IsValid() const { return m_ID != 0; }

    private:
        std::string ReadFile(const std::filesystem::path& path);
        void        CheckErrors(const std::string& typeStr);

        uint32_t   m_ID   = 0;
        ShaderType m_Type = ShaderType::Vertex;
    };

} // namespace Engine::Graphics