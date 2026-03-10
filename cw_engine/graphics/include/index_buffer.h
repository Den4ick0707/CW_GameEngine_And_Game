#ifndef CW_ENGINE_GRAPHICS_INDEX_BUFFER_H
#define CW_ENGINE_GRAPHICS_INDEX_BUFFER_H

#include <cstdint> // Обов'язково для uint32_t

namespace Engine::Graphics {

    /// @brief OpenGL Element Buffer Object (EBO/IBO).
    /// @details
    /// Index Buffers allow us to reuse vertices. Instead of duplicating vertex data
    /// (position, color, etc.) for connected triangles, we store unique vertices in a VBO,
    /// and use this buffer to tell OpenGL the order in which to connect them.
    ///
    /// Example: A square needs 2 triangles.
    /// Without IBO: 6 vertices (2 duplicates).
    /// With IBO: 4 unique vertices + 6 small integers (indices).
    class IndexBuffer {
    public:
        /// @brief Creates and fills the Index Buffer with data.
        /// @param indices Pointer to the array of indices (must be unsigned int).
        /// @param count The TOTAL NUMBER of indices (not the size in bytes!).
        IndexBuffer(uint32_t *indices, uint32_t count);

        /// @brief Deletes the buffer from GPU memory.
        ~IndexBuffer();

        /// @brief Binds this buffer as GL_ELEMENT_ARRAY_BUFFER.
        void Bind() const;

        /// @brief Unbinds the buffer.
        void Unbind() const;

        /// @brief Returns the number of indices in this buffer.
        /// @note This is needed for glDrawElements(..., count, ...).
        inline uint32_t GetCount() const { return m_Count; }

    private:
        uint32_t m_ID;
        uint32_t m_Count;
    };
}

#endif // CW_ENGINE_GRAPHICS_INDEX_BUFFER_H