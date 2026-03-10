#ifndef CW_ENGINE_GRAPHICS_RENDER_COMMAND_H
#define CW_ENGINE_GRAPHICS_RENDER_COMMAND_H

#include "vertex_array.h"
#include <glm/glm.hpp>
#include <memory>
#include <cstdint>

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
        /// @param indexCount Кількість індексів для малювання. Якщо 0, малюються всі індекси з буфера.
        static void DrawIndexed(const std::shared_ptr<VertexArray> &vertexArray, uint32_t indexCount = 0);
    };
}

#endif // CW_ENGINE_GRAPHICS_RENDER_COMMAND_H