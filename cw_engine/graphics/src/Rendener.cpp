#include "Rendener.h"

namespace Engine::Graphics {

    // Ініціалізація статичних даних
    Renderer::SceneData* Renderer::s_SceneData = new Renderer::SceneData;

    void Renderer::BeginScene(const OrthographicCamera& camera) {
        // Зберігаємо матрицю камери на весь поточний кадр
        s_SceneData->ViewProjectionMatrix = camera.GetViewProjectionMatrix();
    }

    void Renderer::EndScene() {
        // Поки що тут пусто. У майбутньому тут буде логіка для Batch Rendering.
    }

    void Renderer::Submit(const std::shared_ptr<ShaderProgram>& shader, 
                          const std::shared_ptr<VertexArray>& vertexArray, 
                          const glm::mat4& transform) 
    {
        // 1. Активуємо шейдер
        shader->Bind();

        // 2. Передаємо матрицю камери (як світ виглядає)
        // ВАЖЛИВО: Твій шейдер ПОВИНЕН мати uniform mat4 u_ViewProjection;
        shader->SetMat4("u_ViewProjection", s_SceneData->ViewProjectionMatrix);

        // 3. Передаємо матрицю об'єкта (де об'єкт знаходиться у світі)
        // ВАЖЛИВО: Твій шейдер ПОВИНЕН мати uniform mat4 u_Transform;
        shader->SetMat4("u_Transform", transform);

        // 4. Активуємо геометрію
        vertexArray->Bind();

        // 5. Даємо команду відеокарті намалювати це
        RenderCommand::DrawIndexed(vertexArray);
    }
}