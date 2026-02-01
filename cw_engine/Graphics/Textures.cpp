#define STB_IMAGE_IMPLEMENTATION
#include "EngineLibraries/stb/stb_image.h"
#include "Textures.h"

namespace Graphics {
    Textures::Textures(const char *texture_path) {
        glGenTextures(1, &t_ID);
        stbi_set_flip_vertically_on_load(true);

        unsigned char *data = stbi_load(texture_path, &t_width, &t_height, &t_nrChannels, 0);

        if (data) {
            GLenum format;
            if (t_nrChannels == 1)
                format = GL_RED;
            else if (t_nrChannels == 3)
                format = GL_RGB;
            else if (t_nrChannels == 4)
                format = GL_RGBA;

            glBindTexture(GL_TEXTURE_2D, t_ID);
            glTexImage2D(GL_TEXTURE_2D, 0, format, t_width, t_height, 0, format, GL_UNSIGNED_BYTE, data);


            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        } else {
            std::cout << "Failed to load texture: " << texture_path << std::endl;
        }
        stbi_image_free(data);
    }

    Textures::Textures(uint32_t width, uint32_t height, const void *data)
        : t_width(width), t_height(height) {
        glGenTextures(1, &t_ID);
        glBindTexture(GL_TEXTURE_2D, t_ID);

        // Налаштування для піксельної графіки та системних текстур
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // Важливо для пікселів
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        // Завантажуємо дані (GL_RGBA8 - стандарт)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

        // Відв'язуємо
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    Textures::~Textures() {
        glDeleteTextures(1, &t_ID);
    }

    void Textures::Bind(unsigned int slot) const {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, t_ID);
    }

    void Textures::Unbind() const {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void Textures::SetFilter(GLenum minFilter, GLenum magFilter) {
        // 1. Прив'язуємо текстуру, щоб OpenGL знав, що ми налаштовуємо саме її
        glBindTexture(GL_TEXTURE_2D, t_ID);

        // 2. Встановлюємо параметри
        // Minifying: коли об'єкт менший за текстуру (вдалині)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);

        // Magnifying: коли об'єкт більший за текстуру (дуже близько, камера в стіні)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);

        // 3. (Опціонально) Відв'язуємо, щоб нічого випадково не зламати
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void Textures::SetWrap(GLenum wrapS, GLenum wrapT) {
        glBindTexture(GL_TEXTURE_2D, t_ID);

        // S - це вісь X на текстурі
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);

        // T - це вісь Y на текстурі
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);

        glBindTexture(GL_TEXTURE_2D, 0);
    }
}
