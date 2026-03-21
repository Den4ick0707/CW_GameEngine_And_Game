#pragma once

#include <string>
#include <cstdint>

namespace Engine::Graphics {

    enum class TextureFilter { Linear, Nearest };
    enum class TextureWrap   { Repeat, ClampToEdge, MirroredRepeat };

    struct TextureSettings {
        TextureFilter MinFilter = TextureFilter::Nearest;
        TextureFilter MagFilter = TextureFilter::Nearest;
        TextureWrap   WrapS     = TextureWrap::Repeat;
        TextureWrap   WrapT     = TextureWrap::Repeat;
        bool          GenMipmaps = true;
    };

    /// @brief 2D текстура OpenGL.
    /// @details Підтримує завантаження з файлу (stb_image)
    /// та створення з масиву пікселів (для білого 1x1 пікселя, FBO тощо).
    class Texture {
    public:
        /// @brief Завантажити з файлу (.png, .jpg, .bmp).
        explicit Texture(const std::string& path,
                         const TextureSettings& settings = {});

        /// @brief Створити з масиву пікселів (або порожню для FBO).
        Texture(uint32_t width, uint32_t height,
                const void* data     = nullptr,
                const TextureSettings& settings = {});

        ~Texture();

        Texture(const Texture&)            = delete;
        Texture& operator=(const Texture&) = delete;

        void Bind(uint32_t slot = 0) const;
        void Unbind() const;

        /// @brief Оновити дані вже існуючої текстури.
        void SetData(const void* data, uint32_t sizeBytes);

        [[nodiscard]] uint32_t GetWidth()      const { return m_Width;      }
        [[nodiscard]] uint32_t GetHeight()     const { return m_Height;     }
        [[nodiscard]] uint32_t GetRendererID() const { return m_RendererID; }
        [[nodiscard]] bool     IsValid()       const { return m_RendererID != 0; }
        [[nodiscard]] const std::string& GetPath() const { return m_Path; }

    private:
        void Create(uint32_t width, uint32_t height,
                    uint32_t internalFmt, uint32_t dataFmt,
                    const void* data, const TextureSettings& settings);
        void ApplySettings(const TextureSettings& settings);

        uint32_t    m_RendererID   = 0;
        uint32_t    m_Width        = 0;
        uint32_t    m_Height       = 0;
        uint32_t    m_InternalFmt  = 0;
        uint32_t    m_DataFmt      = 0;
        std::string m_Path;
    };

} // namespace Engine::Graphics