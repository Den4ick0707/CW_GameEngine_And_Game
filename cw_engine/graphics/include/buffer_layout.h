#pragma once

#include <vector>
#include <string>
#include <cstdint>
#include <initializer_list>

namespace Engine::Graphics {
    /// @brief Типи даних атрибутів шейдера.
    enum class ShaderDataType {
        None = 0,
        Float, Float2, Float3, Float4,
        Mat3, Mat4,
        Int, Int2, Int3, Int4,
        Bool
    };

    /// @brief Розмір у байтах для кожного ShaderDataType.
    inline uint32_t ShaderDataTypeSize(ShaderDataType type) {
        switch (type) {
            case ShaderDataType::Float: return 4;
            case ShaderDataType::Float2: return 4 * 2;
            case ShaderDataType::Float3: return 4 * 3;
            case ShaderDataType::Float4: return 4 * 4;
            case ShaderDataType::Mat3: return 4 * 3 * 3;
            case ShaderDataType::Mat4: return 4 * 4 * 4;
            case ShaderDataType::Int: return 4;
            case ShaderDataType::Int2: return 4 * 2;
            case ShaderDataType::Int3: return 4 * 3;
            case ShaderDataType::Int4: return 4 * 4;
            case ShaderDataType::Bool: return 1;
            case ShaderDataType::None: return 0;
        }
        return 0;
    }

    /// @brief Один атрибут вершини (позиція, колір, UV тощо).
    struct BufferElement {
        std::string Name;
        ShaderDataType Type = ShaderDataType::None;
        uint32_t Size = 0;
        uint32_t Offset = 0;
        bool Normalized = false;

        BufferElement() = default;

        BufferElement(ShaderDataType type,
                      const std::string &name,
                      bool normalized = false)
            : Name(name)
              , Type(type)
              , Size(ShaderDataTypeSize(type))
              , Normalized(normalized) {
        }

        /// @brief Кількість базових компонентів (Float3 → 3, Mat4 → 16).
        [[nodiscard]] uint32_t GetComponentCount() const {
            switch (Type) {
                case ShaderDataType::Float: return 1;
                case ShaderDataType::Float2: return 2;
                case ShaderDataType::Float3: return 3;
                case ShaderDataType::Float4: return 4;
                case ShaderDataType::Mat3: return 9;
                case ShaderDataType::Mat4: return 16;
                case ShaderDataType::Int: return 1;
                case ShaderDataType::Int2: return 2;
                case ShaderDataType::Int3: return 3;
                case ShaderDataType::Int4: return 4;
                case ShaderDataType::Bool: return 1;
                case ShaderDataType::None: return 0;
            }
            return 0;
        }
    };

    /// @brief Опис розкладки вершинного буфера.
    /// @details Автоматично обчислює Stride і Offset для кожного елемента.
    ///
    /// @code
    /// BufferLayout layout = {
    ///     { ShaderDataType::Float3, "a_Position" },
    ///     { ShaderDataType::Float4, "a_Color"    },
    ///     { ShaderDataType::Float2, "a_TexCoord" },
    ///     { ShaderDataType::Float,  "a_TexIndex" }
    /// };
    /// @endcode
    class BufferLayout {
    public:
        BufferLayout() = default;

        BufferLayout(std::initializer_list<BufferElement> elements)
            : m_Elements(elements) {
            Calculate();
        }

        [[nodiscard]] uint32_t GetStride() const { return m_Stride; }
        [[nodiscard]] const std::vector<BufferElement> &GetElements() const { return m_Elements; }
        [[nodiscard]] bool IsEmpty() const { return m_Elements.empty(); }

        auto begin() { return m_Elements.begin(); }
        auto end() { return m_Elements.end(); }
        auto begin() const { return m_Elements.begin(); }
        auto end() const { return m_Elements.end(); }

    private:
        void Calculate() {
            uint32_t offset = 0;
            m_Stride = 0;
            for (auto &e: m_Elements) {
                e.Offset = offset;
                offset += e.Size;
                m_Stride += e.Size;
            }
        }

        std::vector<BufferElement> m_Elements;
        uint32_t m_Stride = 0;
    };
} // namespace Engine::Graphics
