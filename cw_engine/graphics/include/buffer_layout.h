#pragma once

#include <initializer_list>
#include <vector>
#include <string>
#include <cstdint>

namespace Engine::Graphics {

    /// @brief Defines the data types supported by the shader attributes.
    enum class ShaderDataType {
        None = 0,
        Float, Float2, Float3, Float4,
        Mat3, Mat4,
        Int, Int2, Int3, Int4,
        Bool
    };

    /// @brief Returns the size in bytes of a ShaderDataType.
    // FIX: inline замість static — уникаємо дублювання у кожній одиниці трансляції
    inline uint32_t GetShaderDataTypeSize(ShaderDataType type) {
        switch (type) {
            case ShaderDataType::Float:  return 4;
            case ShaderDataType::Float2: return 4 * 2;
            case ShaderDataType::Float3: return 4 * 3;
            case ShaderDataType::Float4: return 4 * 4;
            case ShaderDataType::Mat3:   return 4 * 3 * 3;
            case ShaderDataType::Mat4:   return 4 * 4 * 4;
            case ShaderDataType::Int:    return 4;
            case ShaderDataType::Int2:   return 4 * 2;
            case ShaderDataType::Int3:   return 4 * 3;
            case ShaderDataType::Int4:   return 4 * 4;
            case ShaderDataType::Bool:   return 1;
            case ShaderDataType::None:   return 0;
        }
        return 0;
    }

    /// @brief Represents a single vertex attribute (e.g., Position, Color, TexCoord).
    struct BufferElement {
        std::string    Name;
        ShaderDataType Type;
        uint32_t       Size;
        uint32_t       Offset;
        bool           Normalized;

        BufferElement() = default;

        BufferElement(ShaderDataType type, const std::string& name, bool normalized = false)
            : Name(name), Type(type),
              Size(GetShaderDataTypeSize(type)), Offset(0), Normalized(normalized) {}

        uint32_t GetComponentCount() const {
            switch (Type) {
                case ShaderDataType::Float:  return 1;
                case ShaderDataType::Float2: return 2;
                case ShaderDataType::Float3: return 3;
                case ShaderDataType::Float4: return 4;
                case ShaderDataType::Mat3:   return 3 * 3;
                case ShaderDataType::Mat4:   return 4 * 4;
                case ShaderDataType::Int:    return 1;
                case ShaderDataType::Int2:   return 2;
                case ShaderDataType::Int3:   return 3;
                case ShaderDataType::Int4:   return 4;
                case ShaderDataType::Bool:   return 1;
                case ShaderDataType::None:   return 0;
            }
            return 0;
        }
    };

    /// @brief Describes the memory layout of a Vertex Buffer.
    /// @details Automatically calculates Stride and per-element Offsets.
    ///
    /// @code
    /// BufferLayout layout = {
    ///     { ShaderDataType::Float3, "a_Position" },
    ///     { ShaderDataType::Float4, "a_Color" }
    /// };
    /// @endcode
    class BufferLayout {
    public:
        BufferLayout() = default;

        BufferLayout(const std::initializer_list<BufferElement>& elements)
            : m_Elements(elements) {
            CalculateOffsetsAndStride();
        }

        [[nodiscard]] uint32_t GetStride() const { return m_Stride; }
        [[nodiscard]] const std::vector<BufferElement>& GetElements() const { return m_Elements; }

        std::vector<BufferElement>::iterator       begin()       { return m_Elements.begin(); }
        std::vector<BufferElement>::iterator       end()         { return m_Elements.end();   }
        std::vector<BufferElement>::const_iterator begin() const { return m_Elements.begin(); }
        std::vector<BufferElement>::const_iterator end()   const { return m_Elements.end();   }

    private:
        void CalculateOffsetsAndStride() {
            uint32_t offset = 0;
            m_Stride = 0;
            for (auto& element : m_Elements) {
                element.Offset  = offset;
                offset         += element.Size;
                m_Stride       += element.Size;
            }
        }

    private:
        std::vector<BufferElement> m_Elements;
        uint32_t m_Stride = 0;
    };
}