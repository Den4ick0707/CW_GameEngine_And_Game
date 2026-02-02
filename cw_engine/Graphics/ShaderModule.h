#pragma once
#include "pch.h"
#include <filesystem>

namespace Engine::Graphics {

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
        ~ShaderModule();

        // Забороняємо копіювання, щоб не видалити шейдер двічі
        ShaderModule(const ShaderModule& other) = delete;
        ShaderModule& operator=(const ShaderModule& other) = delete;

        [[nodiscard]] ShaderType GetType() const { return m_Type; }
        [[nodiscard]] uint32_t GetID() const { return m_ID; }

    private:
        uint32_t m_ID = 0;
        ShaderType m_Type;

        std::string ReadFromFile(const std::filesystem::path& path);
        void CheckCompileErrors(uint32_t shader, const std::string& type);
    };
}