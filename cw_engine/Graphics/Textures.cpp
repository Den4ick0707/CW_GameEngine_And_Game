#include "pch.h"
#include "Textures.h"
#include <glad/glad.h>

#define STB_IMAGE_IMPLEMENTATION
#include <../stb/stb_image.h> // Переконайся, що шлях правильний відносно include directories

namespace Engine::Graphics {

    Textures::Textures(const std::string& path)
        : m_Path(path)
    {
        stbi_set_flip_vertically_on_load(true);

        unsigned char* data = stbi_load(path.c_str(), &m_Width, &m_Height, &m_Channels, 0);

        if (data) {
            GLenum internalFormat = 0, dataFormat = 0;
            if (m_Channels == 4) {
                internalFormat = GL_RGBA8;
                dataFormat = GL_RGBA;
            } else if (m_Channels == 3) {
                internalFormat = GL_RGB8;
                dataFormat = GL_RGB;
            }

            glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
            glTextureStorage2D(m_RendererID, 1, internalFormat, m_Width, m_Height);

            // Налаштування фільтрації (Pixel art style)
            glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);

            glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, dataFormat, GL_UNSIGNED_BYTE, data);

            stbi_image_free(data);
        } else {
            std::cerr << "Failed to load texture: " << path << std::endl;
        }
    }

    // Спеціальний конструктор для 1x1 пікселя (білий квадрат)
    Textures::Textures(uint32_t width, uint32_t height, uint32_t data)
        : m_Width(width), m_Height(height)
    {
        glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
        glTextureStorage2D(m_RendererID, 1, GL_RGBA8, m_Width, m_Height);

        glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, GL_RGBA, GL_UNSIGNED_BYTE, &data);
    }

    Textures::~Textures() {
        glDeleteTextures(1, &m_RendererID);
    }

    void Textures::Bind(uint32_t slot) const {
        glBindTextureUnit(slot, m_RendererID);
    }

    void Textures::Unbind() const {
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}