#pragma once

#include "render_command.h"
#include "OrthographicCamera.h"
#include "shader_program.h"
#include <glm/glm.hpp>
#include <memory>

namespace Engine::Graphics {

    /// @brief High-level scene renderer.
    class Renderer {
    public:
        /// @brief Begins the scene, accepting camera settings.
        static void BeginScene(const OrthographicCamera& camera);

        /// @brief Ends the scene.
        static void EndScene();

        /// @brief Submits an object for rendering.
        /// @param shader      Shader program for this object.
        /// @param vertexArray Object geometry.
        /// @param transform   Transform matrix (position, rotation, scale) in world space.
        static void Submit(const std::shared_ptr<ShaderProgram>& shader,
                           const std::shared_ptr<VertexArray>& vertexArray,
                           const glm::mat4& transform = glm::mat4(1.0f));

    private:
        struct SceneData {
            glm::mat4 ViewProjectionMatrix;
        };

        // FIX: використовуємо значення замість голого вказівника — немає витоку пам'яті
        static SceneData s_SceneData;
    };
}