#ifndef CW_ENGINE_GRAPHICS_TEXTURE_2D_H
#define CW_ENGINE_GRAPHICS_TEXTURE_2D_H

#include <string>
#include <cstdint>

namespace Engine::Graphics {

    /// @brief Режими фільтрації текстури
    enum class TextureFilter {
        Linear,  ///< Плавне розмиття (для HD графіки, UI)
        Nearest  ///< Чіткі пікселі (для Pixel Art)
    };

    /// @brief Режими поведінки текстури на краях
    enum class TextureWrap {
        Repeat,      ///< Повторювати текстуру
        ClampToEdge  ///< Розтягувати крайні пікселі (корисно для UI)
    };

    /// @brief Налаштування текстури при створенні
    struct TextureSettings {
        TextureFilter MinFilter = TextureFilter::Nearest;
        TextureFilter MagFilter = TextureFilter::Nearest;
        TextureWrap WrapS = TextureWrap::Repeat;
        TextureWrap WrapT = TextureWrap::Repeat;
    };

    /// @brief Клас для роботи з 2D текстурами
    class Texture {
    public:
        /// @brief Завантажує текстуру з файлу (.png, .jpg)
        Texture(const std::string& path, const TextureSettings& settings = TextureSettings());

        /// @brief Створює порожню текстуру або текстуру з масиву пікселів
        /// @param data Вказівник на масив пікселів (може бути nullptr для порожньої текстури)
        Texture(uint32_t width, uint32_t height, const void* data = nullptr, const TextureSettings& settings = TextureSettings());

        ~Texture();

        Texture(const Texture&) = delete;
        Texture& operator=(const Texture&) = delete;

        /// @brief Активує текстуру у вказаному слоті
        void Bind(uint32_t slot = 0) const;

        /// @brief Деактивує текстуру
        void Unbind() const;

        /// @brief Оновлює частину або всю текстуру новими даними
        void SetData(const void* data, uint32_t size);

        [[nodiscard]] uint32_t GetWidth() const { return m_Width; }
        [[nodiscard]] uint32_t GetHeight() const { return m_Height; }
        [[nodiscard]] uint32_t GetRendererID() const { return m_RendererID; }

    private:
        /// @brief Допоміжна функція для застосування налаштувань OpenGL
        void ApplySettings(const TextureSettings& settings);

    private:
        uint32_t m_RendererID = 0;
        uint32_t m_Width = 0;
        uint32_t m_Height = 0;
        uint32_t m_Channels = 0;

        uint32_t m_InternalFormat = 0;
        uint32_t m_DataFormat = 0;

        std::string m_Path;
    };
}

#endif // CW_ENGINE_GRAPHICS_TEXTURE_H