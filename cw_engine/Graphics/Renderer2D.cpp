#include "pch.h"
#include "Renderer2D.h"

#include "VertexArray.h"
#include "ShaderProgram.h"
#include "Textures.h"
#include "ShaderModule.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Engine {

    // Структура для збереження стану рендерера
    struct Renderer2DData {
        std::shared_ptr<Graphics::VertexArray> QuadVertexArray;
        std::shared_ptr<Graphics::ShaderProgram> TextureShader;
        std::shared_ptr<Graphics::Textures> WhiteTexture;
    };

    static Renderer2DData* s_Data;

    void Renderer2D::Init() {
        s_Data = new Renderer2DData();

        // 1. Створення VAO
        s_Data->QuadVertexArray = std::make_shared<Graphics::VertexArray>();

        // Вершини: Позиція (3) + Текстурні координати (2)
        float vertices[5 * 4] = {
            -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
             0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
             0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
            -0.5f,  0.5f, 0.0f, 0.0f, 1.0f
        };

        auto vertexBuffer = std::make_shared<Graphics::VertexBuffer>(vertices, sizeof(vertices));

        Graphics::BufferLayout layout = {
            { Graphics::ShaderDataType::Float3, "a_Position" },
            { Graphics::ShaderDataType::Float2, "a_TexCoord" }
        };
        vertexBuffer->SetLayout(layout);
        s_Data->QuadVertexArray->AddVertexBuffer(vertexBuffer);

        uint32_t indices[6] = { 0, 1, 2, 2, 3, 0 };
        auto indexBuffer = std::make_shared<Graphics::IndexBuffer>(indices, 6);
        s_Data->QuadVertexArray->SetIndexBuffer(indexBuffer);

        // 2. Створення Білої Текстури (Програмно!)
        // 0xffffffff = R(ff) G(ff) B(ff) A(ff) -> Білий колір
        uint32_t whiteTextureData = 0xffffffff;
        s_Data->WhiteTexture = std::make_shared<Graphics::Textures>(1, 1, whiteTextureData);

        // 3. Завантаження Шейдерів
        // Переконайся, що ці файли існують у папці з .exe (ми налаштували це в CMake)
        s_Data->TextureShader = std::make_shared<Graphics::ShaderProgram>();

        Graphics::ShaderModule vert("Resource/Shaders/DefaultShader.vert", Graphics::ShaderType::VERTEX);
        Graphics::ShaderModule frag("Resource/Shaders/DefaultShader.frag", Graphics::ShaderType::FRAGMENT);

        s_Data->TextureShader->AttachShader(vert);
        s_Data->TextureShader->AttachShader(frag);
        s_Data->TextureShader->Link();

        s_Data->TextureShader->Bind();
        s_Data->TextureShader->SetInt("u_Texture", 0); // Слот текстури за замовчуванням
    }

    void Renderer2D::Shutdown() {
        delete s_Data;
    }

    void Renderer2D::BeginScene(const Graphics::OrthographicCamera& camera) {
        s_Data->TextureShader->Bind();
        s_Data->TextureShader->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix());
    }

    void Renderer2D::EndScene() {
        // Тут буде Batch Rendering flush, коли дійдемо до оптимізації
    }

    // --- Реалізація Drawing ---

    void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color) {
        DrawQuad({ position.x, position.y, 0.0f }, size, color);
    }

    void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color) {
        // Створюємо матрицю трансформації "на льоту"
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
            * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

        DrawQuad(transform, color);
    }

    void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const std::shared_ptr<Graphics::Textures>& texture) {
        DrawQuad({ position.x, position.y, 0.0f }, size, texture);
    }

    void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const std::shared_ptr<Graphics::Textures>& texture) {
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
            * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

        DrawQuad(transform, texture);
    }

    // --- Matrix Overloads (The Core Logic) ---

    void Renderer2D::DrawQuad(const glm::mat4& transform, const glm::vec4& color) {
        // Малюємо просто колір = малюємо білу текстуру, помножену на колір
        DrawQuad(transform, s_Data->WhiteTexture, color);
    }

    void Renderer2D::DrawQuad(const glm::mat4& transform, const std::shared_ptr<Graphics::Textures>& texture, const glm::vec4& tintColor) {
        s_Data->TextureShader->Bind();
        s_Data->TextureShader->SetFloat4("u_Color", tintColor);
        s_Data->TextureShader->SetMat4("u_Transform", transform);

        // Біндимо текстуру (якщо texture == nullptr, це краш, але в нас завжди є WhiteTexture або передана)
        if (texture)
            texture->Bind(0);
        else
            s_Data->WhiteTexture->Bind(0); // Страховка

        s_Data->QuadVertexArray->Bind();
        glDrawElements(GL_TRIANGLES, s_Data->QuadVertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
    }

}