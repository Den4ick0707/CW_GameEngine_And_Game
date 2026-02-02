#pragma once
#include "pch.h"
#include "OrthographicCamera.h"
#include "Textures.h"
#include <glm/glm.hpp>
#include <memory>

namespace Engine {

    class Renderer2D {
    public:
        static void Init();
        static void Shutdown();

        static void BeginScene(const Graphics::OrthographicCamera& camera);
        static void EndScene();

        // --- Basic Drawing (Legacy support) ---
        static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
        static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color);
        static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const std::shared_ptr<Graphics::Textures>& texture);
        static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const std::shared_ptr<Graphics::Textures>& texture);

        // --- Matrix Drawing (FOR GAMEOBJECTS) ---
        // Це те, що використовує наша система компонентів
        static void DrawQuad(const glm::mat4& transform, const glm::vec4& color);
        static void DrawQuad(const glm::mat4& transform, const std::shared_ptr<Graphics::Textures>& texture, const glm::vec4& tintColor = glm::vec4(1.0f));
    };

}