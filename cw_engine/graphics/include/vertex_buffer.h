#pragma once

#include "buffer_layout.h"
#include <cstdint>

namespace Engine::Graphics {

    /// @brief OpenGL Vertex Buffer Object (VBO).
    /// @details Два режими використання:
    ///   - Static  : дані відомі одразу, не змінюються (GL_STATIC_DRAW).
    ///   - Dynamic : буфер виділяється наперед, дані оновлюються через SetData (GL_DYNAMIC_DRAW).
    class VertexBuffer {
    public:
        /// @brief Dynamic VBO — виділяє пам'ять без даних.
        explicit VertexBuffer(uint32_t size);

        /// @brief Static VBO — одразу завантажує дані.
        VertexBuffer(const void* vertices, uint32_t size);

        ~VertexBuffer();

        VertexBuffer(const VertexBuffer&)            = delete;
        VertexBuffer& operator=(const VertexBuffer&) = delete;

        void Bind()   const;
        void Unbind() const;

        /// @brief Оновити частину буфера (тільки для Dynamic VBO).
        void SetData(const void* data, uint32_t size);

        [[nodiscard]] const BufferLayout& GetLayout() const { return m_Layout; }
        void SetLayout(const BufferLayout& layout)          { m_Layout = layout; }

        [[nodiscard]] uint32_t GetRendererID() const { return m_RendererID; }

    private:
        uint32_t     m_RendererID = 0;
        BufferLayout m_Layout;
    };

} // namespace Engine::Graphics