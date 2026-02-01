#ifndef OPENGLPROJECT_TEXTURES_HPP
#define OPENGLPROJECT_TEXTURES_HPP
#include "pch.h"
namespace Graphics {
    class Textures {
    public:
        unsigned int t_ID;
        int t_width, t_height, t_nrChannels;

        Textures(const char *path);
        Textures(uint32_t width, uint32_t height, const void* data);
        ~Textures();

        void Bind(unsigned int slot = 0) const;

        void Unbind() const;

        // minFilter: як текстура виглядає, коли вона далеко (зменшена)
        // magFilter: як текстура виглядає, коли ми дуже близько (збільшена)
        void SetFilter(GLenum minFilter, GLenum magFilter);

        // wrapS: по горизонталі (вісь X/U)
        // wrapT: по вертикалі (вісь Y/V)
        void SetWrap(GLenum wrapS, GLenum wrapT);
    };
}
#endif //OPENGLPROJECT_TEXTURES_HPP
