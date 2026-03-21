#pragma once

#include "vertex_buffer.h"
#include "index_buffer.h"
#include <vector>
#include <memory>

namespace Engine::Graphics {

    /// @brief OpenGL Vertex Array Object (VAO).
    /// @details Зберігає конфігурацію атрибутів вершин.
    /// Після налаштування достатньо викликати Bind() перед малюванням.
    class VertexArray {
    public:
        VertexArray();
        ~VertexArray();

        VertexArray(const VertexArray&)            = delete;
        VertexArray& operator=(const VertexArray&) = delete;

        void Bind()   const;
        void Unbind() const;

        /// @brief Додати VBO і автоматично налаштувати атрибути за його Layout.
        void AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vbo);

        /// @brief Встановити IBO (прив'язується до VAO стану).
        void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& ibo);

        [[nodiscard]] const std::vector<std::shared_ptr<VertexBuffer>>& GetVertexBuffers() const { return m_VBOs; }
        [[nodiscard]] const std::shared_ptr<IndexBuffer>&               GetIndexBuffer()   const { return m_IBO; }

        [[nodiscard]] uint32_t GetRendererID() const { return m_RendererID; }

    private:
        uint32_t m_RendererID        = 0;
        uint32_t m_AttribIndex       = 0;   // поточний location для glVertexAttribPointer

        std::vector<std::shared_ptr<VertexBuffer>> m_VBOs;
        std::shared_ptr<IndexBuffer>               m_IBO;
    };

} // namespace Engine::Graphics