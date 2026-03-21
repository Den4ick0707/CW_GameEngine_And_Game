#include "texture_2d.h"
#include <glad/glad.h>
#include <iostream>

// STB_IMAGE_IMPLEMENTATION має бути рівно в одному .cpp файлі
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Engine::Graphics {

    static GLenum ToGLFilter(TextureFilter f) {
        return f == TextureFilter::Linear ? GL_LINEAR : GL_NEAREST;
    }

    static GLenum ToGLWrap(TextureWrap w) {
        switch (w) {
            case TextureWrap::Repeat:         return GL_REPEAT;
            case TextureWrap::ClampToEdge:    return GL_CLAMP_TO_EDGE;
            case TextureWrap::MirroredRepeat: return GL_MIRRORED_REPEAT;
        }
        return GL_REPEAT;
    }

    // ── З файлу ──────────────────────────────────────────────────────────────

    Texture::Texture(const std::string& path, const TextureSettings& settings)
        : m_Path(path)
    {
        stbi_set_flip_vertically_on_load(1);

        int w, h, ch;
        unsigned char* data = stbi_load(path.c_str(), &w, &h, &ch, 0);

        if (!data) {
            std::cerr << "[Texture] Failed to load: " << path << "\n";
            return;
        }

        uint32_t internalFmt = 0, dataFmt = 0;
        if      (ch == 4) { internalFmt = GL_RGBA8; dataFmt = GL_RGBA; }
        else if (ch == 3) { internalFmt = GL_RGB8;  dataFmt = GL_RGB;  }
        else if (ch == 1) { internalFmt = GL_R8;    dataFmt = GL_RED;  }
        else {
            std::cerr << "[Texture] Unsupported channel count: " << ch << "\n";
            stbi_image_free(data);
            return;
        }

        Create(static_cast<uint32_t>(w), static_cast<uint32_t>(h),
               internalFmt, dataFmt, data, settings);

        stbi_image_free(data);
    }

    // ── З масиву пікселів ─────────────────────────────────────────────────────

    Texture::Texture(uint32_t width, uint32_t height,
                     const void* data, const TextureSettings& settings)
    {
        Create(width, height, GL_RGBA8, GL_RGBA, data, settings);
    }

    // ── Загальна ініціалізація ────────────────────────────────────────────────

    void Texture::Create(uint32_t width, uint32_t height,
                         uint32_t internalFmt, uint32_t dataFmt,
                         const void* data, const TextureSettings& settings)
    {
        m_Width       = width;
        m_Height      = height;
        m_InternalFmt = internalFmt;
        m_DataFmt     = dataFmt;

        glGenTextures(1, &m_RendererID);
        glBindTexture(GL_TEXTURE_2D, m_RendererID);

        ApplySettings(settings);

        glTexImage2D(GL_TEXTURE_2D, 0,
                     static_cast<GLint>(m_InternalFmt),
                     static_cast<GLsizei>(m_Width),
                     static_cast<GLsizei>(m_Height),
                     0, m_DataFmt, GL_UNSIGNED_BYTE, data);

        if (settings.GenMipmaps && data)
            glGenerateMipmap(GL_TEXTURE_2D);

        glBindTexture(GL_TEXTURE_2D, 0);
    }

    Texture::~Texture() {
        if (m_RendererID)
            glDeleteTextures(1, &m_RendererID);
    }

    void Texture::ApplySettings(const TextureSettings& s) {
        GLenum minFilter = ToGLFilter(s.MinFilter);
        // Якщо міпмапи — використовуємо LINEAR_MIPMAP_LINEAR для min filter
        if (s.GenMipmaps && s.MinFilter == TextureFilter::Linear)
            minFilter = GL_LINEAR_MIPMAP_LINEAR;

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(minFilter));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, static_cast<GLint>(ToGLFilter(s.MagFilter)));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     static_cast<GLint>(ToGLWrap(s.WrapS)));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     static_cast<GLint>(ToGLWrap(s.WrapT)));
    }

    void Texture::SetData(const void* data, uint32_t sizeBytes) {
        uint32_t bpp = (m_DataFmt == GL_RGBA) ? 4 : (m_DataFmt == GL_RGB) ? 3 : 1;
        if (sizeBytes != m_Width * m_Height * bpp) {
            std::cerr << "[Texture] SetData size mismatch!\n";
            return;
        }
        glBindTexture(GL_TEXTURE_2D, m_RendererID);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
                        static_cast<GLsizei>(m_Width),
                        static_cast<GLsizei>(m_Height),
                        m_DataFmt, GL_UNSIGNED_BYTE, data);
    }

    void Texture::Bind(uint32_t slot) const {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, m_RendererID);
    }

    void Texture::Unbind() const {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

} // namespace Engine::Graphics