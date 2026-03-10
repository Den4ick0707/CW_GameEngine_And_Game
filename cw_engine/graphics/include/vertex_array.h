#ifndef CW_ENGINE_GRAPHICS_VERTEX_ARRAY_H
#define CW_ENGINE_GRAPHICS_VERTEX_ARRAY_H

#include "vertex_buffer.h"
#include "index_buffer.h" // Переконайся, що цей файл існує
#include <vector>
#include <memory>

namespace Engine::Graphics {
    /// @brief A container object that holds references to Vertex Buffers and an Index Buffer.
    /// @details
    /// The Vertex Array Object (VAO) stores the state of vertex attribute configurations.
    /// Instead of setting up layout (glVertexAttribPointer) every frame, you do it once,
    /// and then just Bind() the VAO.
    ///
    /// Usage flow:
    /// 1. Create VAO.
    /// 2. Bind VAO.
    /// 3. Create VBO -> Set Layout -> Add to VAO.
    /// 4. Create IBO -> Set to VAO.
    class VertexArray {
    public:
        /// @brief Creates an empty Vertex Array Object on the GPU.
        VertexArray();

        /// @brief Destroys the VAO and frees GPU memory.
        ~VertexArray();

        /// @brief Binds this VAO. All subsequent draw calls will use this configuration.
        void Bind() const;

        /// @brief Unbinds the VAO (binds 0).
        void Unbind() const;

        /// @brief Adds a Vertex Buffer and configures its attributes based on its layout.
        /// @param vertexBuffer Shared pointer to the VBO. The VAO keeps a reference to it.
        void AddVertexBuffer(const std::shared_ptr<VertexBuffer> &vertexBuffer);

        /// @brief Sets the Index Buffer for this VAO.
        /// @note The Element Array Buffer binding is stored within the VAO state.
        /// @param indexBuffer Shared pointer to the IBO.
        void SetIndexBuffer(const std::shared_ptr<IndexBuffer> &indexBuffer);

        /// @brief Returns the list of attached Vertex Buffers.
        const std::vector<std::shared_ptr<VertexBuffer> > &GetVertexBuffers() const { return m_VertexBuffers; }

        /// @brief Returns the attached Index Buffer.
        const std::shared_ptr<IndexBuffer> &GetIndexBuffer() const { return m_IndexBuffer; }

    private:
        uint32_t m_ID; ///< OpenGL Render ID.
        uint32_t m_VertexBufferIndex = 0; ///< Tracks the current attribute index (location) for the shader.

        std::vector<std::shared_ptr<VertexBuffer> > m_VertexBuffers;
        std::shared_ptr<IndexBuffer> m_IndexBuffer;
    };
}

#endif // CW_ENGINE_GRAPHICS_VERTEX_ARRAY_H
