#include "vertex_array.h"
#include <glad/glad.h> // Обов'язково!

namespace Engine::Graphics {
    static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type) {
        switch (type) {
            case ShaderDataType::Float: return GL_FLOAT;
            case ShaderDataType::Float2: return GL_FLOAT;
            case ShaderDataType::Float3: return GL_FLOAT;
            case ShaderDataType::Float4: return GL_FLOAT;
            case ShaderDataType::Mat3: return GL_FLOAT;
            case ShaderDataType::Mat4: return GL_FLOAT;
            case ShaderDataType::Int: return GL_INT;
            case ShaderDataType::Int2: return GL_INT;
            case ShaderDataType::Int3: return GL_INT;
            case ShaderDataType::Int4: return GL_INT;
            case ShaderDataType::Bool: return GL_BOOL;
            case ShaderDataType::None: break;
        }
        return 0;
    }

    VertexArray::VertexArray() {
        glGenVertexArrays(1, &m_ID);
    }

    VertexArray::~VertexArray() {
        glDeleteVertexArrays(1, &m_ID);
    }

    void VertexArray::Bind() const {
        glBindVertexArray(m_ID);
    }

    void VertexArray::Unbind() const {
        glBindVertexArray(0);
    }

    void VertexArray::AddVertexBuffer(const  std::shared_ptr<VertexBuffer> &vertexBuffer)  {
        glBindVertexArray(m_ID);
        vertexBuffer->Bind();

        const auto &layout = vertexBuffer->GetLayout();
        for (const auto &element: layout) {
            switch (element.Type) {
                case ShaderDataType::Float:
                case ShaderDataType::Float2:
                case ShaderDataType::Float3:
                case ShaderDataType::Float4: {
                    glEnableVertexAttribArray(m_VertexBufferIndex);
                    glVertexAttribPointer(
                        m_VertexBufferIndex,
                        element.GetComponentCount(),
                        ShaderDataTypeToOpenGLBaseType(element.Type),
                        element.Normalized ? GL_TRUE : GL_FALSE,
                        layout.GetStride(),
                        (const void *) (uintptr_t) element.Offset
                    );
                    m_VertexBufferIndex++;
                    break;
                }
                case ShaderDataType::Int:
                case ShaderDataType::Int2:
                case ShaderDataType::Int3:
                case ShaderDataType::Int4:
                case ShaderDataType::Bool: {
                    glEnableVertexAttribArray(m_VertexBufferIndex);
                    glVertexAttribIPointer(
                        m_VertexBufferIndex,
                        element.GetComponentCount(),
                        ShaderDataTypeToOpenGLBaseType(element.Type),
                        layout.GetStride(),
                        (const void *) (uintptr_t) element.Offset
                    );
                    m_VertexBufferIndex++;
                    break;
                }
                case ShaderDataType::Mat3:
                case ShaderDataType::Mat4: {
                    // Матриці поки пропускаємо або треба реалізувати цикл на 4 ітерації
                    // CW_ERROR_LOG("Matrices not supported yet in simple AddVertexBuffer");
                    break;
                }
                default:
                    break;
            }
        }

        m_VertexBuffers.push_back(vertexBuffer);
    }

    void VertexArray::SetIndexBuffer(const std::shared_ptr<IndexBuffer> &indexBuffer) {
        glBindVertexArray(m_ID);
        indexBuffer->Bind();
        m_IndexBuffer = indexBuffer;
    }
}
