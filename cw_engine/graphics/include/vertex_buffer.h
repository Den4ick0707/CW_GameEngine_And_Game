#ifndef CW_ENGINE_GRAPHICS_VERTEX_BUFFER_H
#define CW_ENGINE_GRAPHICS_VERTEX_BUFFER_H

#include "buffer_layout.h"
#include <cstdint>

namespace Engine::Graphics {

    /// @brief A wrapper around an OpenGL Vertex Buffer Object (VBO).
    /// @details
    /// This class manages memory on the GPU dedicated to storing vertex data
    /// (Positions, Colors, Texture Coordinates, Normals, etc.).
    ///
    /// There are two ways to initialize it:
    /// 1. Static (Constructor with data): For objects that don't change (e.g., a house).
    /// 2. Dynamic (Constructor with size): For objects that change every frame (e.g., particles, batch renderer).
    class VertexBuffer {
    public:
        /// @brief Creates a dynamic vertex buffer.
        /// @details Allocates memory on the GPU without initializing it.
        /// Use this for dynamic geometry where you call SetData() frequently.
        ///
        /// @param size The size of the buffer in bytes.
        VertexBuffer(uint32_t size);

        /// @brief Creates and fills a static vertex buffer.
        /// @details Uploads data to the GPU immediately. Ideally used for GL_STATIC_DRAW.
        ///
        /// @param vertices Pointer to the array of data (float array or struct array).
        /// @param size The total size of the data in bytes.
        ///
        /// @code
        /// float vertices[] = { -0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f, ... };
        /// auto vbo = std::make_shared<VertexBuffer>(vertices, sizeof(vertices));
        /// @endcode
        VertexBuffer(const void *vertices, uint32_t size);

        /// @brief Deletes the VBO from VRAM (glDeleteBuffers).
        ~VertexBuffer();

        /// @brief Binds this buffer as the current GL_ARRAY_BUFFER.
        /// @details All subsequent vertex attribute calls will affect this buffer.
        void Bind() const;

        /// @brief Unbinds the currently bound vertex buffer (binds 0).
        void Unbind() const;

        /// @brief Updates a portion (or all) of the buffer's data.
        /// @details Uses glBufferSubData. Only works if the buffer was allocated previously.
        ///
        /// @param data Pointer to the new data.
        /// @param size Size of the data in bytes.
        void SetData(const void *data, uint32_t size);

        /// @brief Gets the buffer layout (structure of vertex attributes).
        const BufferLayout &GetLayout() const { return m_Layout; }

        /// @brief Sets the layout.
        /// @details This is critical! The VertexArray needs this to know how to interpret the bytes.
        ///
        /// @param layout The layout definition.
        void SetLayout(const BufferLayout &layout) { m_Layout = layout; }

    private:
        uint32_t m_RendererID; ///< The OpenGL ID of the buffer.
        BufferLayout m_Layout; ///< The layout description (Stride, Offsets, Types).
    };
}
#endif // CW_ENGINE_GRAPHICS_VERTEX_BUFFER_H