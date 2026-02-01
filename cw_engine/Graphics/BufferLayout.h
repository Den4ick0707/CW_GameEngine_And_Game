#ifndef OPENGLPROJECT_BUFFERLAYOUT_HPP
#define OPENGLPROJECT_BUFFERLAYOUT_HPP
#include "pch.h"
namespace Graphics {
    enum ShaderDataType {
        NONE = 0,
        Float, Float2, Float3, Float4,
        Mat3, Mat4,
        Int, Int2, Int3, Int4,
        Bool
    };

    static uint32_t GetShaderDataTypeSize(ShaderDataType type) {
        switch (type) {
            case ShaderDataType::Float: return sizeof(float);
            case ShaderDataType::Float2: return sizeof(float) * 2;
            case ShaderDataType::Float3: return sizeof(float) * 3;
            case ShaderDataType::Float4: return sizeof(float) * 4;
            case ShaderDataType::Mat3: return 4 * 3 * 3;
            case ShaderDataType::Mat4: return 4 * 4 * 4;
            case ShaderDataType::Int: return sizeof(int);
            case ShaderDataType::Int2: return sizeof(int) * 2;
            case ShaderDataType::Int3: return sizeof(int) * 3;
            case ShaderDataType::Int4: return sizeof(int) * 4;
            case ShaderDataType::Bool: return sizeof(bool);
        }
        return 0;
    }


    struct BufferElement {
        std::string Name;
        ShaderDataType Type;
        uint32_t Size;
        uint32_t Offset;
        bool Normalized;

        BufferElement(ShaderDataType type, const std::string name, bool normalised = false)
            : Name(name), Type(type), Size(GetShaderDataTypeSize(type)), Offset(0), Normalized(normalised) {
        }

        uint32_t GetComponentCount() const {
            switch (Type) {
                case ShaderDataType::Float: return 1;
                case ShaderDataType::Float2: return 2;
                case ShaderDataType::Float3: return 3;
                case ShaderDataType::Float4: return 4;
                case ShaderDataType::Mat3: return 3 * 3;
                case ShaderDataType::Mat4: return 4 * 4;
                case ShaderDataType::Int: return 1;
                case ShaderDataType::Int2: return 2;
                case ShaderDataType::Int3: return 3;
                case ShaderDataType::Int4: return 4;
                case ShaderDataType::Bool: return 1;
            }
            return 0;
        }
    };

    class BufferLayout {
        std::vector<BufferElement> m_Elements;
        uint32_t m_Stride = 0;

        void CalculateOffsetsAndStride() {
            uint32_t offset = 0;
            m_Stride = 0;
            for (auto &element: m_Elements) {
                element.Offset = offset;
                offset += element.Size;
                m_Stride += element.Size;
            }
        }

    public:
        BufferLayout() {
        }

        BufferLayout(const std::initializer_list<BufferElement> &elements)
            : m_Elements(elements) {
            CalculateOffsetsAndStride();
        }

        inline uint32_t GetStride() const { return m_Stride; }
        inline const std::vector<BufferElement> &GetElements() const { return m_Elements; }


        std::vector<BufferElement>::iterator begin() { return m_Elements.begin(); }
        std::vector<BufferElement>::iterator end() { return m_Elements.end(); }
        std::vector<BufferElement>::const_iterator begin() const { return m_Elements.begin(); }
        std::vector<BufferElement>::const_iterator end() const { return m_Elements.end(); }
    };
}
#endif //OPENGLPROJECT_BUFFERLAYOUT_HPP
