#ifndef COURSEWORK_RENDERER2D_H
#define COURSEWORK_RENDERER2D_H
#pragma once
#include "pch.h"
#include "Graphics/OrthographicCamera.h"
#include "Graphics/Textures.h"

namespace Engine {
    class Renderer2D {
    public:
        // Ініціалізація (створення VAO, VBO, дефолтного шейдера)
        static void Init();

        // Очищення пам'яті
        static void Shutdown();

        // Початок сцени (передаємо камеру, щоб шейдер знав матрицю ViewProjection)
        static void BeginScene(const Graphics::OrthographicCamera &camera);

        // Кінець сцени (тут можна робити batch rendering, поки просто пусто)
        static void EndScene();

        // --- Малювання примітивів ---


        static void DrawQuad(const glm::vec2 &position, const glm::vec2 &size, const glm::vec4 &color);

        static void DrawQuad(const glm::vec3 &position, const glm::vec2 &size, const glm::vec4 &color);

        // 2. Квадрат з текстурою
        static void DrawQuad(const glm::vec2 &position, const glm::vec2 &size,
                             const std::shared_ptr<Graphics::Textures> &texture);

        static void DrawQuad(const glm::vec3 &position, const glm::vec2 &size,
                             const std::shared_ptr<Graphics::Textures> &texture);

        // 3. Квадрат з текстурою + тінт кольору (наприклад, червонуватий спрайт при ударі)
        static void DrawQuad(const glm::vec2 &position, const glm::vec2 &size,
                             const std::shared_ptr<Graphics::Textures> &texture, const glm::vec4 &tintColor);

        static void DrawQuad(const glm::vec3 &position, const glm::vec2 &size,
                             const std::shared_ptr<Graphics::Textures> &texture, const glm::vec4 &tintColor);

        // 4. Поворот (Rotation) - можна додати окремий метод DrawRotatedQuad
        static void DrawRotatedQuad(const glm::vec2 &position, const glm::vec2 &size, float rotation,
                                    const glm::vec4 &color);

        static void DrawRotatedQuad(const glm::vec2 &position, const glm::vec2 &size, float rotation,
                                    const std::shared_ptr<Graphics::Textures> &texture,
                                    const glm::vec4 &tintColor = glm::vec4(1.0f));
    };
}
#endif //COURSEWORK_RENDERER2D_H
