#ifndef OPENGLPROJECT_INDEXBUFFEROBJECT_HPP
#define OPENGLPROJECT_INDEXBUFFEROBJECT_HPP
#include "pch.h"
#include <cstdint>
namespace Graphics {
    class IndexBuffer {
    public:
        IndexBuffer(uint32_t *indices, uint32_t count);
        ~IndexBuffer();

        void Bind() const;

        void Unbind() const;

        uint32_t GetCount() const { return m_Count; }

    private:
        uint32_t m_RendererID;
        uint32_t m_Count;
    };
}
#endif //OPENGLPROJECT_INDEXBUFFEROBJECT_HPP
