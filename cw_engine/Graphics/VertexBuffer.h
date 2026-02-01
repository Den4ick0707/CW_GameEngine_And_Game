#ifndef OPENGLPROJECT_VERTEXBUFFEROBJECT_HPP
#define OPENGLPROJECT_VERTEXBUFFEROBJECT_HPP
#include "pch.h"
#include "BufferLayout.h"
namespace Graphics {
    class VertexBuffer {
    public:
        VertexBuffer(const void* data, uint32_t size);
        VertexBuffer(uint32_t size);

        ~VertexBuffer();

        void Bind() const;
        void Unbind() const;

        void SetData(const void* data, uint32_t size);

        const BufferLayout& GetLayout() const { return m_Layout; }
        void SetLayout(const BufferLayout& layout) { m_Layout = layout; }

    private:
        uint32_t m_RendererID;
        BufferLayout m_Layout;
    };
}
#endif //OPENGLPROJECT_VERTEXBUFFEROBJECT_HPP