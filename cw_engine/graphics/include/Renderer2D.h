#pragma once

#include "OrthographicCamera.h"
#include "texture_2d.h"
#include <glm/glm.hpp>
#include <memory>

namespace Engine::Graphics {

    /// @brief Batch renderer для 2D об'єктів.
    /// @details До 10 000 квадратів та 32 текстури за один draw call.
    class Renderer2D {
    public:
        static void Init();
        static void Shutdown();

        static void BeginScene(const OrthographicCamera& camera);
        static void EndScene();

        // ── DrawQuad — кольоровий ─────────────────────────────────────────
        static void DrawQuad(const glm::vec2& pos, const glm::vec2& size,
                             const glm::vec4& color);
        static void DrawQuad(const glm::vec3& pos, const glm::vec2& size,
                             const glm::vec4& color);

        // ── DrawQuad — текстурований ──────────────────────────────────────
        static void DrawQuad(const glm::vec2& pos, const glm::vec2& size,
                             const std::shared_ptr<Texture>& tex,
                             const glm::vec4& tint = glm::vec4(1.0f));
        static void DrawQuad(const glm::vec3& pos, const glm::vec2& size,
                             const std::shared_ptr<Texture>& tex,
                             const glm::vec4& tint = glm::vec4(1.0f));

        // ── DrawQuad — з поворотом ────────────────────────────────────────
        static void DrawRotatedQuad(const glm::vec3& pos, const glm::vec2& size,
                                    float rotationDeg, const glm::vec4& color);
        static void DrawRotatedQuad(const glm::vec3& pos, const glm::vec2& size,
                                    float rotationDeg,
                                    const std::shared_ptr<Texture>& tex,
                                    const glm::vec4& tint = glm::vec4(1.0f));

        // ── Статистика ────────────────────────────────────────────────────
        struct Statistics {
            uint32_t DrawCalls = 0;
            uint32_t QuadCount = 0;
            uint32_t VertexCount() const { return QuadCount * 4; }
            uint32_t IndexCount()  const { return QuadCount * 6; }
        };

        static void       ResetStats();
        static Statistics GetStats();
        static void FlushAndReset();


    private:
        static void Flush();
        static void SubmitQuad(const glm::mat4& transform,
                               const glm::vec4& color,
                               float texIndex);
    };

} // namespace Engine::Graphics