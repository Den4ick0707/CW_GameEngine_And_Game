#pragma once

#include "vertex_array.h"
#include <glm/glm.hpp>
#include <memory>
#include <cstdint>

namespace Engine::Graphics {

    /// @brief Тонка обгортка над low-level командами OpenGL.
    class RenderCommand {
    public:
        static void Init();

        static void SetClearColor(const glm::vec4& color);
        static void Clear();

        /// @brief glDrawElements. indexCount=0 → весь IBO.
        static void DrawIndexed(const std::shared_ptr<VertexArray>& vao,
                                uint32_t indexCount = 0);

        static void SetDepthTest(bool enabled);
        static void SetBlend    (bool enabled);
        static void SetWireframe(bool enabled);

        [[nodiscard]] static uint32_t GetMaxTextureSlots();
    };

} // namespace Engine::Graphics