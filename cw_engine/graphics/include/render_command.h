#ifndef CW_ENGINE_GRAPHICS_RENDER_COMMAND_H
#define CW_ENGINE_GRAPHICS_RENDER_COMMAND_H


#include "vertex_array.h"
#include <glm/glm.hpp>
#include <memory>

namespace Engine::Graphics {
    /// @brief Низькорівнева обгортка над командами OpenGL.
    class RenderCommand {
    public:
        /// @brief Встановлює колір очищення екрану (фон).
        static void SetClearColor(const glm::vec4 &color);

        /// @brief Очищує буфери кольору та глибини.
        static void Clear();

        /// @brief Малює геометрію на основі індексного буфера (glDrawElements).
        /// @param vertexArray VAO, який містить VBO та IBO.
        static void DrawIndexed(const std::shared_ptr<VertexArray> &vertexArray);
    };
}

#endif // CW_ENGINE_GRAPHICS_RENDER_COMMAND_H
