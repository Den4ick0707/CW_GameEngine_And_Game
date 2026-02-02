#pragma once
#include "pch.h"
#include <string>

namespace Engine::Graphics {

    class Textures {
    public:
        Textures(const std::string& path);
        // Для створення "білої текстури" вручну
        Textures(uint32_t width, uint32_t height, uint32_t data);
        ~Textures();

        void Bind(uint32_t slot = 0) const;
        void Unbind() const;

        uint32_t GetWidth() const { return m_Width; }
        uint32_t GetHeight() const { return m_Height; }
        uint32_t GetRendererID() const { return m_RendererID; }

    private:
        uint32_t m_RendererID;
        int m_Width, m_Height, m_Channels;
        std::string m_Path;
    };
}