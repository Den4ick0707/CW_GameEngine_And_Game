#pragma once

#include "OrthographicCamera.h"
#include "texture_2d.h"
#include <glm/glm.hpp>
#include <memory>

namespace Engine::Graphics {

    class Renderer2D {
    public:
        static void Init();
        static void Shutdown();

        static void BeginScene(const OrthographicCamera& camera);
        static void EndScene();

        // --- Кольоровий квадрат (texIndex = 0 → білий піксель, отже колір не змінюється) ---
        static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
        static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color);

        // --- Текстурований квадрат ---
        static void DrawQuad(const glm::vec2& position, const glm::vec2& size,
                             const std::shared_ptr<Texture>& texture,
                             const glm::vec4& tint = { 1.0f, 1.0f, 1.0f, 1.0f });

        static void DrawQuad(const glm::vec3& position, const glm::vec2& size,
                             const std::shared_ptr<Texture>& texture,
                             const glm::vec4& tint = { 1.0f, 1.0f, 1.0f, 1.0f });

        // --- Статистика (для ImGui overlay) ---
        struct Statistics {
            uint32_t DrawCalls = 0;
            uint32_t QuadCount = 0;
        };

        static void      ResetStats();
        static Statistics GetStats();

    private:
        static void Flush();
        static void FlushAndReset();
    };

}
