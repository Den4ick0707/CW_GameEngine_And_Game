#include "Renderer2D.h"
#include "vertex_array.h"
#include "vertex_buffer.h"
#include "index_buffer.h"
#include "shader_program.h"
#include "shader_module.h"
#include "render_command.h"
#include "buffer_layout.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <array>

namespace Engine::Graphics {

    // =========================================================================
    // Структура однієї вершини квадрата
    // =========================================================================
    struct QuadVertex {
        glm::vec3 Position;   // location = 0
        glm::vec4 Color;      // location = 1
        glm::vec2 TexCoord;   // location = 2  (NEW)
        float     TexIndex;   // location = 3  (NEW) — індекс слоту текстури
    };

    // =========================================================================
    // Внутрішній стан рендерера
    // =========================================================================
    struct Renderer2DData {
        // Ліміти одного батчу
        static constexpr uint32_t MaxQuads        = 10000;
        static constexpr uint32_t MaxVertices     = MaxQuads * 4;
        static constexpr uint32_t MaxIndices      = MaxQuads * 6;
        static constexpr uint32_t MaxTextureSlots = 32; // обмеження OpenGL (зазвичай 16–32)

        // GPU об'єкти
        std::shared_ptr<VertexArray>   QuadVAO;
        std::shared_ptr<VertexBuffer>  QuadVBO;
        std::shared_ptr<ShaderProgram> FlatColorShader;

        // CPU-сторона буфера вершин
        uint32_t    QuadIndexCount       = 0;
        QuadVertex* QuadVertexBufferBase = nullptr; // початок масиву
        QuadVertex* QuadVertexBufferPtr  = nullptr; // поточна позиція запису

        // Слоти текстур для поточного батчу
        // Слот 0 завжди = білий 1×1 піксель (використовується кольоровими квадратами)
        std::array<std::shared_ptr<Texture>, MaxTextureSlots> TextureSlots;
        uint32_t TextureSlotIndex = 1; // 0 зарезервований

        // UV-кути для квадрата (порядок: лівий нижній → правий нижній → правий верхній → лівий верхній)
        static constexpr glm::vec4 QuadVertexPositions[4] = {
            { -0.5f, -0.5f, 0.0f, 1.0f },
            {  0.5f, -0.5f, 0.0f, 1.0f },
            {  0.5f,  0.5f, 0.0f, 1.0f },
            { -0.5f,  0.5f, 0.0f, 1.0f }
        };

        static constexpr glm::vec2 QuadUVCoords[4] = {
            { 0.0f, 0.0f },
            { 1.0f, 0.0f },
            { 1.0f, 1.0f },
            { 0.0f, 1.0f }
        };

        // Статистика
        Renderer2D::Statistics Stats;
    };

    static Renderer2DData s_Data;

    // =========================================================================
    // Init / Shutdown
    // =========================================================================

    void Renderer2D::Init() {
        // --- VAO + динамічний VBO ---
        s_Data.QuadVAO = std::make_shared<VertexArray>();

        s_Data.QuadVBO = std::make_shared<VertexBuffer>(
            s_Data.MaxVertices * sizeof(QuadVertex)
        );
        s_Data.QuadVBO->SetLayout({
            { ShaderDataType::Float3, "a_Position" },
            { ShaderDataType::Float4, "a_Color"    },
            { ShaderDataType::Float2, "a_TexCoord" }, // NEW
            { ShaderDataType::Float,  "a_TexIndex" }  // NEW
        });
        s_Data.QuadVAO->AddVertexBuffer(s_Data.QuadVBO);

        // CPU-буфер вершин
        s_Data.QuadVertexBufferBase = new QuadVertex[s_Data.MaxVertices];

        // --- Статичний IBO (індекси не змінюються, тільки кількість) ---
        uint32_t* quadIndices = new uint32_t[s_Data.MaxIndices];
        uint32_t offset = 0;
        for (uint32_t i = 0; i < s_Data.MaxIndices; i += 6) {
            quadIndices[i + 0] = offset + 0;
            quadIndices[i + 1] = offset + 1;
            quadIndices[i + 2] = offset + 2;
            quadIndices[i + 3] = offset + 2;
            quadIndices[i + 4] = offset + 3;
            quadIndices[i + 5] = offset + 0;
            offset += 4;
        }
        auto quadIBO = std::make_shared<IndexBuffer>(quadIndices, s_Data.MaxIndices);
        s_Data.QuadVAO->SetIndexBuffer(quadIBO);
        delete[] quadIndices;

        // --- Білий 1×1 піксель у слот 0 ---
        // Завдяки цьому кольоровий DrawQuad просто множить колір на білий → без змін
        uint32_t whitePixelData = 0xFFFFFFFF;
        s_Data.TextureSlots[0] = std::make_shared<Texture>(1, 1, &whitePixelData);

        // --- Шейдер ---
        s_Data.FlatColorShader = std::make_shared<ShaderProgram>();
        ShaderModule vertShader("res/shaders/basic.vert", ShaderType::VERTEX);
        ShaderModule fragShader("res/shaders/basic.frag", ShaderType::FRAGMENT);
        s_Data.FlatColorShader->AttachShader(vertShader);
        s_Data.FlatColorShader->AttachShader(fragShader);
        s_Data.FlatColorShader->Link();

        // Передаємо масив sampler-ів один раз — вони не змінюються
        // (прив'язка текстур до слотів змінюється, але індекси — ні)
        s_Data.FlatColorShader->Bind();
        int samplers[s_Data.MaxTextureSlots];
        for (int i = 0; i < (int)s_Data.MaxTextureSlots; i++) samplers[i] = i;
        s_Data.FlatColorShader->SetIntArray("u_Textures", samplers, s_Data.MaxTextureSlots);
    }

    void Renderer2D::Shutdown() {
        delete[] s_Data.QuadVertexBufferBase;
        s_Data.QuadVertexBufferBase = nullptr;
        s_Data.QuadVertexBufferPtr  = nullptr;
        // TextureSlots автоматично звільняються (shared_ptr)
    }

    // =========================================================================
    // BeginScene / EndScene
    // =========================================================================

    void Renderer2D::BeginScene(const OrthographicCamera& camera) {
        s_Data.FlatColorShader->Bind();
        s_Data.FlatColorShader->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix());

        // Скидаємо лічильники батчу
        s_Data.QuadIndexCount      = 0;
        s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;
        s_Data.TextureSlotIndex    = 1; // слот 0 = білий піксель, не чіпаємо
    }

    void Renderer2D::EndScene() {
        Flush();
    }

    // =========================================================================
    // Flush — відправити поточний батч на GPU
    // =========================================================================

    void Renderer2D::Flush() {
        if (s_Data.QuadIndexCount == 0) return;

        // Розраховуємо розмір даних у байтах і заливаємо на GPU
        uint32_t dataSize = static_cast<uint32_t>(
            reinterpret_cast<uint8_t*>(s_Data.QuadVertexBufferPtr) -
            reinterpret_cast<uint8_t*>(s_Data.QuadVertexBufferBase)
        );
        s_Data.QuadVBO->SetData(s_Data.QuadVertexBufferBase, dataSize);

        // Bind усіх задіяних текстурних слотів
        for (uint32_t i = 0; i < s_Data.TextureSlotIndex; i++) {
            if (s_Data.TextureSlots[i])
                s_Data.TextureSlots[i]->Bind(i);
        }

        // Draw call
        s_Data.QuadVAO->Bind();
        glDrawElements(
            GL_TRIANGLES,
            static_cast<GLsizei>(s_Data.QuadIndexCount),
            GL_UNSIGNED_INT,
            nullptr
        );

        // Статистика
        s_Data.Stats.DrawCalls++;
        s_Data.Stats.QuadCount += s_Data.QuadIndexCount / 6;
    }

    void Renderer2D::FlushAndReset() {
        // Скидаємо батч (пряме Flush, НЕ EndScene — щоб уникнути рекурсії)
        Flush();

        s_Data.QuadIndexCount      = 0;
        s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;
        s_Data.TextureSlotIndex    = 1;
    }

    // =========================================================================
    // Внутрішня функція заповнення 4 вершин квадрата
    // =========================================================================

    static void SubmitQuad(const glm::mat4& transform,
                           const glm::vec4& color,
                           const glm::vec2  uvCoords[4],
                           float            texIndex)
    {
        for (int i = 0; i < 4; i++) {
            s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[i];
            s_Data.QuadVertexBufferPtr->Color    = color;
            s_Data.QuadVertexBufferPtr->TexCoord = uvCoords[i];
            s_Data.QuadVertexBufferPtr->TexIndex = texIndex;
            s_Data.QuadVertexBufferPtr++;
        }
        s_Data.QuadIndexCount += 6;
    }

    // =========================================================================
    // DrawQuad — кольорові (texIndex = 0 → білий піксель)
    // =========================================================================

    void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size,
                               const glm::vec4& color)
    {
        DrawQuad({ position.x, position.y, 0.0f }, size, color);
    }

    void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size,
                               const glm::vec4& color)
    {
        if (s_Data.QuadIndexCount >= s_Data.MaxIndices)
            FlushAndReset();

        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
                            * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

        SubmitQuad(transform, color, s_Data.QuadUVCoords, 0.0f);
    }

    // =========================================================================
    // DrawQuad — текстуровані
    // =========================================================================

    void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size,
                               const std::shared_ptr<Texture>& texture,
                               const glm::vec4& tint)
    {
        DrawQuad({ position.x, position.y, 0.0f }, size, texture, tint);
    }

    void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size,
                               const std::shared_ptr<Texture>& texture,
                               const glm::vec4& tint)
    {
        // --- Перевіряємо переповнення батчу ---
        // Перевіряємо і за індексами, і за кількістю слотів
        if (s_Data.QuadIndexCount >= s_Data.MaxIndices ||
            s_Data.TextureSlotIndex >= s_Data.MaxTextureSlots)
        {
            FlushAndReset();
        }

        // --- Знаходимо або реєструємо слот текстури ---
        float texIndex = 0.0f;
        for (uint32_t i = 1; i < s_Data.TextureSlotIndex; i++) {
            // Порівнюємо за OpenGL ID — одна текстура = той самий слот у батчі
            if (s_Data.TextureSlots[i]->GetRendererID() == texture->GetRendererID()) {
                texIndex = static_cast<float>(i);
                break;
            }
        }

        if (texIndex == 0.0f) {
            // Нова текстура — записуємо у наступний вільний слот
            texIndex = static_cast<float>(s_Data.TextureSlotIndex);
            s_Data.TextureSlots[s_Data.TextureSlotIndex] = texture;
            s_Data.TextureSlotIndex++;
        }

        // --- Трансформація на CPU ---
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
                            * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

        SubmitQuad(transform, tint, s_Data.QuadUVCoords, texIndex);
    }

    // =========================================================================
    // Статистика
    // =========================================================================

    void Renderer2D::ResetStats() {
        s_Data.Stats = {};
    }

    Renderer2D::Statistics Renderer2D::GetStats() {
        return s_Data.Stats;
    }

}