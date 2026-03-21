#pragma once

#include <cstdint>

namespace Engine::Graphics {

    /// @brief OpenGL Element Buffer Object (EBO / IBO).
    /// @details Зберігає індекси вершин для glDrawElements.
    /// Дозволяє перевикористовувати вершини без дублювання.
    class IndexBuffer {
    public:
        /// @param indices Масив індексів (uint32_t).
        /// @param count   Кількість індексів (не розмір у байтах!).
        IndexBuffer(const uint32_t* indices, uint32_t count);
        ~IndexBuffer();

        IndexBuffer(const IndexBuffer&)            = delete;
        IndexBuffer& operator=(const IndexBuffer&) = delete;

        void Bind()   const;
        void Unbind() const;

        [[nodiscard]] uint32_t GetCount()      const { return m_Count;      }
        [[nodiscard]] uint32_t GetRendererID() const { return m_RendererID; }

    private:
        uint32_t m_RendererID = 0;
        uint32_t m_Count      = 0;
    };

} // namespace Engine::Graphics