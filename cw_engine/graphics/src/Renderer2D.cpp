#include "Renderer2D.h"
#include "vertex_array.h"
#include "vertex_buffer.h"
#include "index_buffer.h"
#include "shader_program.h"
#include "shader_module.h"
#include "render_command.h"
#include "buffer_layout.h"
#include <glm/gtc/matrix_transform.hpp>

namespace Engine::Graphics {

    struct QuadVertex {
        glm::vec3 Position;
        glm::vec4 Color;
    };

    struct Renderer2DData {
        // FIX: constexpr замість static const — значення повністю видно компілятору,
        // не потребує окремого визначення поза структурою (проблема до C++17)
        static constexpr uint32_t MaxQuads    = 10000;
        static constexpr uint32_t MaxVertices = MaxQuads * 4;
        static constexpr uint32_t MaxIndices  = MaxQuads * 6;

        std::shared_ptr<VertexArray>   QuadVAO;
        std::shared_ptr<VertexBuffer>  QuadVBO;
        std::shared_ptr<ShaderProgram> FlatColorShader;

        uint32_t    QuadIndexCount        = 0;
        QuadVertex* QuadVertexBufferBase  = nullptr;
        QuadVertex* QuadVertexBufferPtr   = nullptr;
    };

    static Renderer2DData s_Data;

    void Renderer2D::Init() {
        s_Data.QuadVAO = std::make_shared<VertexArray>();

        s_Data.QuadVBO = std::make_shared<VertexBuffer>(s_Data.MaxVertices * sizeof(QuadVertex));
        s_Data.QuadVBO->SetLayout({
            { ShaderDataType::Float3, "a_Position" },
            { ShaderDataType::Float4, "a_Color"    }
        });
        s_Data.QuadVAO->AddVertexBuffer(s_Data.QuadVBO);

        s_Data.QuadVertexBufferBase = new QuadVertex[s_Data.MaxVertices];

        // Генеруємо індекси наперед для всіх MaxQuads квадратів
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

        s_Data.FlatColorShader = std::make_shared<ShaderProgram>();
        ShaderModule vertShader("res/shaders/basic.vert", ShaderType::VERTEX);
        ShaderModule fragShader("res/shaders/basic.frag", ShaderType::FRAGMENT);
        s_Data.FlatColorShader->AttachShader(vertShader);
        s_Data.FlatColorShader->AttachShader(fragShader);
        s_Data.FlatColorShader->Link();
    }

    void Renderer2D::Shutdown() {
        delete[] s_Data.QuadVertexBufferBase;
        s_Data.QuadVertexBufferBase = nullptr;
        s_Data.QuadVertexBufferPtr  = nullptr;
    }

    void Renderer2D::BeginScene(const OrthographicCamera& camera) {
        s_Data.FlatColorShader->Bind();
        s_Data.FlatColorShader->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix());

        // FIX: також скидаємо матрицю об'єкту на одиничну —
        // шейдер очікує u_Transform, але в батч-рендері трансформація
        // вже запечена у вершини, тому передаємо identity
        s_Data.FlatColorShader->SetMat4("u_Transform", glm::mat4(1.0f));

        s_Data.QuadIndexCount       = 0;
        s_Data.QuadVertexBufferPtr  = s_Data.QuadVertexBufferBase;
    }

    void Renderer2D::EndScene() {
        Flush();
    }

    void Renderer2D::Flush() {
        if (s_Data.QuadIndexCount == 0) return;

        // Рахуємо розмір даних у байтах
        uint32_t dataSize = static_cast<uint32_t>(
            reinterpret_cast<uint8_t*>(s_Data.QuadVertexBufferPtr) -
            reinterpret_cast<uint8_t*>(s_Data.QuadVertexBufferBase)
        );

        s_Data.QuadVBO->SetData(s_Data.QuadVertexBufferBase, dataSize);

        // FIX: передаємо QuadIndexCount а не весь буфер —
        // оригінальний DrawIndexed малював би всі MaxIndices (60000),
        // навіть якщо батч містить лише 400 квадратів
        s_Data.QuadVAO->Bind();
        glDrawElements(GL_TRIANGLES,
                       static_cast<GLsizei>(s_Data.QuadIndexCount),
                       GL_UNSIGNED_INT,
                       nullptr);
    }

    void Renderer2D::FlushAndReset() {
        EndScene();
        s_Data.QuadIndexCount      = 0;
        s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;
    }

    void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color) {
        DrawQuad({ position.x, position.y, 0.0f }, size, color);
    }

    void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color) {
        if (s_Data.QuadIndexCount >= s_Data.MaxIndices) {
            FlushAndReset();
        }

        // FIX: трансформацію рахуємо тут на CPU і "запікаємо" у вершини.
        // Gemini передавав transform через uniform — це знищує сенс батчингу,
        // бо кожен DrawQuad перезаписував би той самий uniform.
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
                            * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

        constexpr glm::vec4 quadVertexPositions[4] = {
            { -0.5f, -0.5f, 0.0f, 1.0f },
            {  0.5f, -0.5f, 0.0f, 1.0f },
            {  0.5f,  0.5f, 0.0f, 1.0f },
            { -0.5f,  0.5f, 0.0f, 1.0f }
        };

        for (int i = 0; i < 4; i++) {
            s_Data.QuadVertexBufferPtr->Position = transform * quadVertexPositions[i];
            s_Data.QuadVertexBufferPtr->Color    = color;
            s_Data.QuadVertexBufferPtr++;
        }

        s_Data.QuadIndexCount += 6;
    }
}