#include "texture_2d.h"
#include "../../core/include/logger.h"
#include <glad/glad.h>

// FIX: STB_IMAGE_IMPLEMENTATION має бути рівно в одному .cpp файлі
// Шлях виправлено — директорія stb/ має бути додана в CMakeLists через
// target_include_directories, тоді тут просто: #include <stb_image.h>
#define STB_IMAGE_IMPLEMENTATION
#include <../stb/stb_image.h>

namespace Engine::Graphics {

    static GLenum GetGLFilter(TextureFilter filter) {
        return filter == TextureFilter::Linear ? GL_LINEAR : GL_NEAREST;
    }

    static GLenum GetGLWrap(TextureWrap wrap) {
        return wrap == TextureWrap::Repeat ? GL_REPEAT : GL_CLAMP_TO_EDGE;
    }

    Texture::Texture(const std::string& path, const TextureSettings& settings)
        : m_Path(path)
    {
        stbi_set_flip_vertically_on_load(1);

        int width, height, channels;
        unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);

        if (data) {
            m_Width    = static_cast<uint32_t>(width);
            m_Height   = static_cast<uint32_t>(height);
            m_Channels = static_cast<uint32_t>(channels);

            if (channels == 4) {
                m_InternalFormat = GL_RGBA8;
                m_DataFormat     = GL_RGBA;
            } else if (channels == 3) {
                m_InternalFormat = GL_RGB8;
                m_DataFormat     = GL_RGB;
            } else {
                CW_ERROR_LOG("Unsupported texture channel count: {0}", channels);
                stbi_image_free(data);
                return;
            }

            glGenTextures(1, &m_RendererID);
            glBindTexture(GL_TEXTURE_2D, m_RendererID);

            ApplySettings(settings);

            glTexImage2D(GL_TEXTURE_2D, 0, m_InternalFormat,
                         m_Width, m_Height, 0,
                         m_DataFormat, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            stbi_image_free(data);
        } else {
            CW_ERROR_LOG("Failed to load texture: {0}", path);
        }
    }

    Texture::Texture(uint32_t width, uint32_t height, const void* data, const TextureSettings& settings)
        : m_Width(width), m_Height(height),
          m_InternalFormat(GL_RGBA8), m_DataFormat(GL_RGBA)
    {
        glGenTextures(1, &m_RendererID);
        glBindTexture(GL_TEXTURE_2D, m_RendererID);

        ApplySettings(settings);

        // data == nullptr — OpenGL просто виділяє пам'ять (корисно для Framebuffer)
        glTexImage2D(GL_TEXTURE_2D, 0, m_InternalFormat,
                     m_Width, m_Height, 0,
                     m_DataFormat, GL_UNSIGNED_BYTE, data);
    }

    Texture::~Texture() {
        glDeleteTextures(1, &m_RendererID);
    }

    void Texture::ApplySettings(const TextureSettings& settings) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GetGLFilter(settings.MinFilter));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GetGLFilter(settings.MagFilter));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GetGLWrap(settings.WrapS));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GetGLWrap(settings.WrapT));
    }

    void Texture::SetData(const void* data, uint32_t size) {
        uint32_t bpp = (m_DataFormat == GL_RGBA) ? 4 : 3;
        if (size != m_Width * m_Height * bpp) {
            CW_ERROR_LOG("Texture::SetData — size mismatch! Expected {0}, got {1}",
                         m_Width * m_Height * bpp, size);
            return;
        }

        glBindTexture(GL_TEXTURE_2D, m_RendererID);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
                        m_Width, m_Height,
                        m_DataFormat, GL_UNSIGNED_BYTE, data);
    }

    void Texture::Bind(uint32_t slot) const {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, m_RendererID);
    }

    void Texture::Unbind() const {
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}