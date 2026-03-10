#ifndef CW_ENGINE_GRAPHICS_RENDERER_H
#define CW_ENGINE_GRAPHICS_RENDERER_H

#include "render_command.h"
#include "OrthographicCamera.h"
#include "shader_program.h"
#include <glm/glm.hpp>
#include <memory>

namespace Engine::Graphics {

    /// @brief Високорівневий рендерер сцени.
    class Renderer {
    public:
        /// @brief Починає сцену, приймаючи налаштування камери.
        static void BeginScene(const OrthographicCamera& camera);

        /// @brief Завершує сцену.
        static void EndScene();

        /// @brief Відправляє об'єкт на малювання.
        /// @param shader Шейдерна програма для цього об'єкта.
        /// @param vertexArray Геометрія об'єкта.
        /// @param transform Матриця трансформації (позиція, поворот, масштаб) у світі.
        static void Submit(const std::shared_ptr<ShaderProgram>& shader,
                           const std::shared_ptr<VertexArray>& vertexArray,
                           const glm::mat4& transform = glm::mat4(1.0f));

    private:
        struct SceneData {
            glm::mat4 ViewProjectionMatrix;
        };

        static SceneData* s_SceneData;
    };
}

#endif // CW_ENGINE_GRAPHICS_RENDERER_H