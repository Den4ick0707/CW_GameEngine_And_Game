#include "render_command.h"
#include <glad/glad.h>
#include <iostream>

namespace Engine::Graphics {

    void RenderCommand::Init() {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
    }

    void RenderCommand::SetClearColor(const glm::vec4& color) {
        glClearColor(color.r, color.g, color.b, color.a);
    }

    void RenderCommand::Clear() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void RenderCommand::DrawIndexed(const std::shared_ptr<VertexArray>& vao,
                                    uint32_t indexCount)
    {
        vao->Bind();
        uint32_t count = indexCount
            ? indexCount
            : vao->GetIndexBuffer()->GetCount();

        glDrawElements(GL_TRIANGLES,
                       static_cast<GLsizei>(count),
                       GL_UNSIGNED_INT, nullptr);
    }

    void RenderCommand::SetDepthTest(bool enabled) {
        enabled ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
    }

    void RenderCommand::SetBlend(bool enabled) {
        enabled ? glEnable(GL_BLEND) : glDisable(GL_BLEND);
    }

    void RenderCommand::SetWireframe(bool enabled) {
        glPolygonMode(GL_FRONT_AND_BACK, enabled ? GL_LINE : GL_FILL);
    }

    uint32_t RenderCommand::GetMaxTextureSlots() {
        int slots = 0;
        glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &slots);
        return static_cast<uint32_t>(slots);
    }

} // namespace Engine::Graphics