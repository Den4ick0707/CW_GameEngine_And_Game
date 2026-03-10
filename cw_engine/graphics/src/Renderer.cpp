#include "Renderer.h"

namespace Engine::Graphics {

    // FIX: статичне значення замість "new" — без витоку пам'яті
    Renderer::SceneData Renderer::s_SceneData;

    void Renderer::BeginScene(const OrthographicCamera& camera) {
        s_SceneData.ViewProjectionMatrix = camera.GetViewProjectionMatrix();
    }

    void Renderer::EndScene() {
        // Reserved for future Batch Rendering flush logic.
    }

    void Renderer::Submit(const std::shared_ptr<ShaderProgram>& shader,
                          const std::shared_ptr<VertexArray>& vertexArray,
                          const glm::mat4& transform)
    {
        shader->Bind();
        shader->SetMat4("u_ViewProjection", s_SceneData.ViewProjectionMatrix);
        shader->SetMat4("u_Transform", transform);

        vertexArray->Bind();
        RenderCommand::DrawIndexed(vertexArray);
    }
}