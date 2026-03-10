#ifndef CW_ENGINE_GRAPHICS_BUFFER_LAYOUT_H
#define CW_ENGINE_GRAPHICS_BUFFER_LAYOUT_H

#include <initializer_list>
#include <vector>
#include <string>

namespace Engine::Graphics {

    /// @brief Defines the data types supported by the shader attributes.
    enum class ShaderDataType {
        None = 0,
        Float, Float2, Float3, Float4,
        Mat3, Mat4,
        Int, Int2, Int3, Int4,
        Bool
    };

    /// @brief Helper function to get the size in bytes of a specific ShaderDataType.
    /// @param type The shader data type.
    /// @return Size in bytes (e.g., Float3 = 12 bytes).
    static uint32_t GetShaderDataTypeSize(ShaderDataType type) {
        switch (type) {
            case ShaderDataType::Float:    return 4;
            case ShaderDataType::Float2:   return 4 * 2;
            case ShaderDataType::Float3:   return 4 * 3;
            case ShaderDataType::Float4:   return 4 * 4;
            case ShaderDataType::Mat3:     return 4 * 3 * 3;
            case ShaderDataType::Mat4:     return 4 * 4 * 4;
            case ShaderDataType::Int:      return 4;
            case ShaderDataType::Int2:     return 4 * 2;
            case ShaderDataType::Int3:     return 4 * 3;
            case ShaderDataType::Int4:     return 4 * 4;
            case ShaderDataType::Bool:     return 1;
            case ShaderDataType::None:     return 0;
        }
        return 0;
    }

    /// @brief Represents a single attribute in a Vertex Buffer (e.g., Position, Color, TexCoord).
    struct BufferElement {
        std::string Name;       ///< Debug name of the attribute (e.g., "a_Position").
        ShaderDataType Type;    ///< Data type (Float3, Int, etc.).
        uint32_t Size;          ///< Size in bytes (Calculated automatically).
        uint32_t Offset;        ///< Offset in bytes from the start of the vertex (Calculated automatically).
        bool Normalized;        ///< Should non-float data be normalized to [0, 1] by OpenGL?

        BufferElement() = default;

        /// @brief Constructs a BufferElement.
        /// @param type The data type of the attribute.
        /// @param name The name of the attribute (for debugging/shader matching).
        /// @param normalized Set to true if integer data should be normalized.
        BufferElement(ShaderDataType type, const std::string &name, bool normalized = false)
            : Name(name), Type(type), Size(GetShaderDataTypeSize(type)), Offset(0), Normalized(normalized) {
        }

        /// @brief Returns the number of primitive components.
        /// @details Example: Float3 returns 3, Float4 returns 4, Mat4 returns 16 (4*4).
        /// Note: Matrices usually require special handling in VertexArray implementation.
        uint32_t GetComponentCount() const {
            switch (Type) {
                case ShaderDataType::Float:   return 1;
                case ShaderDataType::Float2:  return 2;
                case ShaderDataType::Float3:  return 3;
                case ShaderDataType::Float4:  return 4;
                case ShaderDataType::Mat3:    return 3 * 3;
                case ShaderDataType::Mat4:    return 4 * 4;
                case ShaderDataType::Int:     return 1;
                case ShaderDataType::Int2:    return 2;
                case ShaderDataType::Int3:    return 3;
                case ShaderDataType::Int4:    return 4;
                case ShaderDataType::Bool:    return 1;
                case ShaderDataType::None:    return 0;
            }
            return 0;
        }
    };

    /// @brief Describes the layout of a Vertex Buffer.
    /// @details
    /// This class calculates the Stride and Offsets for all elements automatically.
    /// It allows iterating over elements to configure `glVertexAttribPointer`.
    ///
    /// @code
    /// BufferLayout layout = {
    ///     { ShaderDataType::Float3, "a_Position" },
    ///     { ShaderDataType::Float4, "a_Color" },
    ///     { ShaderDataType::Float2, "a_TexCoord" }
    /// };
    /// vertexBuffer->SetLayout(layout);
    /// @endcode
    class BufferLayout {
    public:
        BufferLayout() {}

        /// @brief Constructs the layout from a list of elements.
        /// @param elements Initializer list of BufferElements.
        BufferLayout(const std::initializer_list<BufferElement> &elements)
            : m_Elements(elements) {
            CalculateOffsetsAndStride();
        }

        /// @brief Gets the total stride (size of one vertex) in bytes.
        inline uint32_t GetStride() const { return m_Stride; }

        /// @brief Gets the list of elements.
        inline const std::vector<BufferElement> &GetElements() const { return m_Elements; }

        // Iterators to support range-based for loops
        std::vector<BufferElement>::iterator begin() { return m_Elements.begin(); }
        std::vector<BufferElement>::iterator end() { return m_Elements.end(); }
        std::vector<BufferElement>::const_iterator begin() const { return m_Elements.begin(); }
        std::vector<BufferElement>::const_iterator end() const { return m_Elements.end(); }

    private:
        /// @brief Calculates the Offset for each element and the total Stride.
        void CalculateOffsetsAndStride() {
            uint32_t offset = 0;
            m_Stride = 0;
            for (auto &element: m_Elements) {
                element.Offset = offset;
                offset += element.Size;
                m_Stride += element.Size;
            }
        }

    private:
        std::vector<BufferElement> m_Elements;
        uint32_t m_Stride = 0;
    };
}

#endif