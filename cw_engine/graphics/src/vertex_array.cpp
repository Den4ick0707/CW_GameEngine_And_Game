#include "vertex_array.h"
#include <glad/glad.h>
#include <cassert>

namespace Engine::Graphics {

    static GLenum ToGLBaseType(ShaderDataType type) {
        switch (type) {
            case ShaderDataType::Float:
            case ShaderDataType::Float2:
            case ShaderDataType::Float3:
            case ShaderDataType::Float4:
            case ShaderDataType::Mat3:
            case ShaderDataType::Mat4:   return GL_FLOAT;
            case ShaderDataType::Int:
            case ShaderDataType::Int2:
            case ShaderDataType::Int3:
            case ShaderDataType::Int4:   return GL_INT;
            case ShaderDataType::Bool:   return GL_BOOL;
            default:                     return 0;
        }
    }

    VertexArray::VertexArray() {
        glCreateVertexArrays(1, &m_RendererID);
    }

    VertexArray::~VertexArray() {
        glDeleteVertexArrays(1, &m_RendererID);
    }

    void VertexArray::Bind()   const { glBindVertexArray(m_RendererID); }
    void VertexArray::Unbind() const { glBindVertexArray(0); }

    void VertexArray::AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vbo) {
        assert(!vbo->GetLayout().IsEmpty() &&
               "VertexBuffer has no layout! Call SetLayout() first.");

        glBindVertexArray(m_RendererID);
        vbo->Bind();

        for (const auto& elem : vbo->GetLayout()) {
            GLenum baseType = ToGLBaseType(elem.Type);

            switch (elem.Type) {
                // Float-типи
                case ShaderDataType::Float:
                case ShaderDataType::Float2:
                case ShaderDataType::Float3:
                case ShaderDataType::Float4: {
                    glEnableVertexAttribArray(m_AttribIndex);
                    glVertexAttribPointer(
                        m_AttribIndex,
                        static_cast<GLint>(elem.GetComponentCount()),
                        baseType,
                        elem.Normalized ? GL_TRUE : GL_FALSE,
                        static_cast<GLsizei>(vbo->GetLayout().GetStride()),
                        reinterpret_cast<const void*>(static_cast<uintptr_t>(elem.Offset)));
                    ++m_AttribIndex;
                    break;
                }
                // Integer-типи (glVertexAttribIPointer — без нормалізації)
                case ShaderDataType::Int:
                case ShaderDataType::Int2:
                case ShaderDataType::Int3:
                case ShaderDataType::Int4:
                case ShaderDataType::Bool: {
                    glEnableVertexAttribArray(m_AttribIndex);
                    glVertexAttribIPointer(
                        m_AttribIndex,
                        static_cast<GLint>(elem.GetComponentCount()),
                        baseType,
                        static_cast<GLsizei>(vbo->GetLayout().GetStride()),
                        reinterpret_cast<const void*>(static_cast<uintptr_t>(elem.Offset)));
                    ++m_AttribIndex;
                    break;
                }
                // Матриці — кожна колонка окремий attrib
                case ShaderDataType::Mat3:
                case ShaderDataType::Mat4: {
                    uint32_t cols = (elem.Type == ShaderDataType::Mat4) ? 4 : 3;
                    for (uint32_t c = 0; c < cols; ++c) {
                        glEnableVertexAttribArray(m_AttribIndex);
                        glVertexAttribPointer(
                            m_AttribIndex,
                            static_cast<GLint>(cols),
                            GL_FLOAT,
                            elem.Normalized ? GL_TRUE : GL_FALSE,
                            static_cast<GLsizei>(vbo->GetLayout().GetStride()),
                            reinterpret_cast<const void*>(
                                static_cast<uintptr_t>(elem.Offset + c * cols * sizeof(float))));
                        glVertexAttribDivisor(m_AttribIndex, 1);
                        ++m_AttribIndex;
                    }
                    break;
                }
                default: break;
            }
        }

        m_VBOs.push_back(vbo);
    }

    void VertexArray::SetIndexBuffer(const std::shared_ptr<IndexBuffer>& ibo) {
        glBindVertexArray(m_RendererID);
        ibo->Bind();
        m_IBO = ibo;
    }

} // namespace Engine::Graphics