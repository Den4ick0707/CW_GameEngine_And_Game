#include "Renderer2D.h"
#include "render_command.h"
#include "shader_program.h"
#include "shader_module.h"
#include "vertex_array.h"
#include "vertex_buffer.h"
#include "index_buffer.h"

#include <glm/gtc/matrix_transform.hpp>
#include <array>

namespace Engine::Graphics {

    struct QuadVertex {
        glm::vec3 Position;
        glm::vec4 Color;
        glm::vec2 TexCoord;
        float     TexIndex;
    };

    static constexpr uint32_t MaxQuads    = 10000;
    static constexpr uint32_t MaxVertices = MaxQuads * 4;
    static constexpr uint32_t MaxIndices  = MaxQuads * 6;
    static constexpr uint32_t MaxTexSlots = 32;

    struct Renderer2DData {
        std::shared_ptr<VertexArray>   QuadVAO;
        std::shared_ptr<VertexBuffer>  QuadVBO;
        std::shared_ptr<ShaderProgram> QuadShader;
        std::shared_ptr<Texture>       WhiteTexture;

        uint32_t    IndexCount       = 0;
        QuadVertex* VertexBufferBase = nullptr;
        QuadVertex* VertexBufferPtr  = nullptr;

        std::array<std::shared_ptr<Texture>, MaxTexSlots> TextureSlots;
        uint32_t TextureSlotIndex = 1;

        glm::mat4 ViewProjection{ 1.0f };

        Renderer2D::Statistics Stats;

        glm::vec4 QuadVertexPositions[4] = {
            { -0.5f, -0.5f, 0.0f, 1.0f },
            {  0.5f, -0.5f, 0.0f, 1.0f },
            {  0.5f,  0.5f, 0.0f, 1.0f },
            { -0.5f,  0.5f, 0.0f, 1.0f }
        };
    };

    static Renderer2DData s_Data;

    // ── Init / Shutdown ───────────────────────────────────────────────────────

    void Renderer2D::Init() {
        s_Data.QuadVAO = std::make_shared<VertexArray>();

        s_Data.QuadVBO = std::make_shared<VertexBuffer>(MaxVertices * (uint32_t)sizeof(QuadVertex));
        s_Data.QuadVBO->SetLayout({
            { ShaderDataType::Float3, "a_Position" },
            { ShaderDataType::Float4, "a_Color"    },
            { ShaderDataType::Float2, "a_TexCoord" },
            { ShaderDataType::Float,  "a_TexIndex" }
        });
        s_Data.QuadVAO->AddVertexBuffer(s_Data.QuadVBO);

        s_Data.VertexBufferBase = new QuadVertex[MaxVertices];

        auto* indices = new uint32_t[MaxIndices];
        uint32_t offset = 0;
        for (uint32_t i = 0; i < MaxIndices; i += 6) {
            indices[i + 0] = offset + 0;
            indices[i + 1] = offset + 1;
            indices[i + 2] = offset + 2;
            indices[i + 3] = offset + 2;
            indices[i + 4] = offset + 3;
            indices[i + 5] = offset + 0;
            offset += 4;
        }
        auto ibo = std::make_shared<IndexBuffer>(indices, MaxIndices);
        s_Data.QuadVAO->SetIndexBuffer(ibo);
        delete[] indices;

        uint32_t white = 0xFFFFFFFF;
        s_Data.WhiteTexture = std::make_shared<Texture>(1, 1, &white);
        s_Data.TextureSlots[0] = s_Data.WhiteTexture;

        s_Data.QuadShader = std::make_shared<ShaderProgram>();
        ShaderModule vert("res/shaders/basic.vert", ShaderType::Vertex);
        ShaderModule frag("res/shaders/basic.frag", ShaderType::Fragment);
        s_Data.QuadShader->AttachShader(vert);
        s_Data.QuadShader->AttachShader(frag);
        s_Data.QuadShader->Link();

        s_Data.QuadShader->Bind();
        int samplers[MaxTexSlots];
        for (int i = 0; i < (int)MaxTexSlots; ++i) samplers[i] = i;
        s_Data.QuadShader->SetIntArray("u_Textures", samplers, MaxTexSlots);
    }

    void Renderer2D::Shutdown() {
        delete[] s_Data.VertexBufferBase;
        s_Data.VertexBufferBase = nullptr;
    }

    // ── Scene ─────────────────────────────────────────────────────────────────

    void Renderer2D::BeginScene(const OrthographicCamera& camera) {
        s_Data.ViewProjection   = camera.GetViewProjectionMatrix();
        s_Data.IndexCount       = 0;
        s_Data.VertexBufferPtr  = s_Data.VertexBufferBase;
        s_Data.TextureSlotIndex = 1;
    }

    void Renderer2D::EndScene() {
        Flush();
    }

    void Renderer2D::Flush() {
        if (s_Data.IndexCount == 0) return;

        uint32_t dataSize = (uint32_t)(
            (uint8_t*)s_Data.VertexBufferPtr - (uint8_t*)s_Data.VertexBufferBase);
        s_Data.QuadVBO->SetData(s_Data.VertexBufferBase, dataSize);

        for (uint32_t i = 0; i < s_Data.TextureSlotIndex; ++i)
            s_Data.TextureSlots[i]->Bind(i);

        s_Data.QuadShader->Bind();
        s_Data.QuadShader->SetMat4("u_ViewProjection", s_Data.ViewProjection);

        // DrawIndexed з явним count щоб не малювати весь IBO
        glDrawElements(GL_TRIANGLES, (int)s_Data.IndexCount, GL_UNSIGNED_INT, nullptr);
        ++s_Data.Stats.DrawCalls;
    }

    void  Renderer2D::FlushAndReset() {
        Flush();
        s_Data.IndexCount       = 0;
        s_Data.VertexBufferPtr  = s_Data.VertexBufferBase;
        s_Data.TextureSlotIndex = 1;
    }

    // ── SubmitQuad (приватний) ────────────────────────────────────────────────

    void Renderer2D::SubmitQuad(const glm::mat4& transform,
                                 const glm::vec4& color,
                                 float texIndex)
    {
        if (s_Data.IndexCount >= MaxIndices)
            FlushAndReset();

        static constexpr glm::vec2 texCoords[4] = {
            {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}
        };

        for (int i = 0; i < 4; ++i) {
            s_Data.VertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[i];
            s_Data.VertexBufferPtr->Color    = color;
            s_Data.VertexBufferPtr->TexCoord = texCoords[i];
            s_Data.VertexBufferPtr->TexIndex = texIndex;
            ++s_Data.VertexBufferPtr;
        }
        s_Data.IndexCount += 6;
        ++s_Data.Stats.QuadCount;
    }

    // ── GetOrAddTexture (локальний helper) ────────────────────────────────────

    static float GetOrAddTexture(const std::shared_ptr<Texture>& tex) {
        for (uint32_t i = 1; i < s_Data.TextureSlotIndex; ++i)
            if (s_Data.TextureSlots[i].get() == tex.get())
                return (float)i;

        if (s_Data.TextureSlotIndex >= MaxTexSlots)
            Renderer2D::FlushAndReset();

        float idx = (float)s_Data.TextureSlotIndex;
        s_Data.TextureSlots[s_Data.TextureSlotIndex++] = tex;
        return idx;
    }

    // ── Public DrawQuad ───────────────────────────────────────────────────────

    void Renderer2D::DrawQuad(const glm::vec2& pos, const glm::vec2& size,
                               const glm::vec4& color) {
        DrawQuad({ pos.x, pos.y, 0.0f }, size, color);
    }

    void Renderer2D::DrawQuad(const glm::vec3& pos, const glm::vec2& size,
                               const glm::vec4& color) {
        glm::mat4 t = glm::translate(glm::mat4(1.0f), pos)
                    * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
        SubmitQuad(t, color, 0.0f);
    }

    void Renderer2D::DrawQuad(const glm::vec2& pos, const glm::vec2& size,
                               const std::shared_ptr<Texture>& tex,
                               const glm::vec4& tint) {
        DrawQuad({ pos.x, pos.y, 0.0f }, size, tex, tint);
    }

    void Renderer2D::DrawQuad(const glm::vec3& pos, const glm::vec2& size,
                               const std::shared_ptr<Texture>& tex,
                               const glm::vec4& tint) {
        glm::mat4 t = glm::translate(glm::mat4(1.0f), pos)
                    * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
        SubmitQuad(t, tint, GetOrAddTexture(tex));
    }

    // ── Public DrawRotatedQuad ────────────────────────────────────────────────

    void Renderer2D::DrawRotatedQuad(const glm::vec3& pos, const glm::vec2& size,
                                      float rotationDeg, const glm::vec4& color) {
        glm::mat4 t = glm::translate(glm::mat4(1.0f), pos)
                    * glm::rotate(glm::mat4(1.0f), glm::radians(rotationDeg), {0.f,0.f,1.f})
                    * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
        SubmitQuad(t, color, 0.0f);
    }

    void Renderer2D::DrawRotatedQuad(const glm::vec3& pos, const glm::vec2& size,
                                      float rotationDeg,
                                      const std::shared_ptr<Texture>& tex,
                                      const glm::vec4& tint) {
        glm::mat4 t = glm::translate(glm::mat4(1.0f), pos)
                    * glm::rotate(glm::mat4(1.0f), glm::radians(rotationDeg), {0.f,0.f,1.f})
                    * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
        SubmitQuad(t, tint, GetOrAddTexture(tex));
    }

    // ── Stats ─────────────────────────────────────────────────────────────────

    Renderer2D::Statistics Renderer2D::GetStats() { return s_Data.Stats; }
    void Renderer2D::ResetStats()                  { s_Data.Stats = {}; }

} // namespace Engine::Graphics