#include "pch.h"
#include "Renderer2D.h"

#include "Graphics/VertexArray.h"
#include "Graphics/ShaderProgram.h"
#include "Graphics/Textures.h"

#include <glm/gtc/matrix_transform.hpp>

#include "ShaderModule.h"

namespace Engine {
    // Внутрішнє сховище даних рендера
    struct Renderer2DData {
        std::shared_ptr<Graphics::VertexArray> QuadVertexArray;
        std::shared_ptr<Graphics::ShaderProgram> TextureShader;
        std::shared_ptr<Graphics::Textures> WhiteTexture; // Для кольорових квадратів без текстури
    };

    static Renderer2DData *s_Data;

    void Renderer2D::Init() {
        s_Data = new Renderer2DData();

        // --- 1. Створення Квадрата (VAO/VBO/IBO) ---
        s_Data->QuadVertexArray = std::make_shared<Graphics::VertexArray>();

        float vertices[5 * 4] = {
            -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
            0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
            0.5f, 0.5f, 0.0f, 1.0f, 1.0f,
            -0.5f, 0.5f, 0.0f, 0.0f, 1.0f
        };

        auto vertexBuffer = std::make_shared<Graphics::VertexBuffer>(vertices, sizeof(vertices));

        Graphics::BufferLayout layout = {
            {Graphics::ShaderDataType::Float3, "a_Position"},
            {Graphics::ShaderDataType::Float2, "a_TexCoord"}
        };
        vertexBuffer->SetLayout(layout);
        s_Data->QuadVertexArray->AddVertexBuffer(vertexBuffer);

        uint32_t indices[6] = {0, 1, 2, 2, 3, 0};
        auto indexBuffer = std::make_shared<Graphics::IndexBuffer>(indices, 6);
        s_Data->QuadVertexArray->SetIndexBuffer(indexBuffer);

        // --- 2. БІЛА ТЕКСТУРА (З ФАЙЛУ) ---
        // Тут ми просто вказуємо шлях. Якщо файл є - все запрацює.
        s_Data->WhiteTexture = std::make_shared<Graphics::Textures>("assets/textures/placeholder.png");

        // --- 3. Шейдери ---
        s_Data->TextureShader = std::make_shared<Graphics::ShaderProgram>();
        Graphics::ShaderModule vert("assets/shaders/DefaultShader.vert", Graphics::ShaderType::VERTEX);
        Graphics::ShaderModule frag("assets/shaders/DefaultShader.frag", Graphics::ShaderType::FRAGMENT);

        s_Data->TextureShader->AttachShader(vert);
        s_Data->TextureShader->AttachShader(frag);
        s_Data->TextureShader->Link();

        s_Data->TextureShader->Bind();
        s_Data->TextureShader->SetInt("u_Texture", 0);
    }

    void Renderer2D::Shutdown() {
        delete s_Data;
    }

    void Renderer2D::BeginScene(const Graphics::OrthographicCamera &camera) {
        s_Data->TextureShader->Bind();
        // Передаємо матрицю камери
        s_Data->TextureShader->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix());
    }

    void Renderer2D::EndScene() {
        // Тут поки нічого
    }

    // --- Реалізація малювання ---

    void Renderer2D::DrawQuad(const glm::vec2 &position, const glm::vec2 &size, const glm::vec4 &color) {
        DrawQuad({position.x, position.y, 0.0f}, size, color);
    }

    void Renderer2D::DrawQuad(const glm::vec3 &position, const glm::vec2 &size, const glm::vec4 &color) {
        // Використовуємо білу текстуру + тінт кольору
        s_Data->TextureShader->SetFloat4("u_Color", color);
        s_Data->WhiteTexture->Bind(0); // Слот 0

        // Рахуємо матрицю трансформації
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(
                                  glm::mat4(1.0f), {size.x, size.y, 1.0f});

        s_Data->TextureShader->SetMat4("u_Transform", transform);

        // Малюємо
        s_Data->QuadVertexArray->Bind();
        glDrawElements(GL_TRIANGLES, s_Data->QuadVertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
    }

    void Renderer2D::DrawQuad(const glm::vec2 &position, const glm::vec2 &size,
                              const std::shared_ptr<Graphics::Textures> &texture) {
        DrawQuad({position.x, position.y, 0.0f}, size, texture, glm::vec4(1.0f));
    }

    void Renderer2D::DrawQuad(const glm::vec3 &position, const glm::vec2 &size,
                              const std::shared_ptr<Graphics::Textures> &texture, const glm::vec4 &tintColor) {
        s_Data->TextureShader->SetFloat4("u_Color", tintColor);
        texture->Bind(0); // Слот 0

        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(
                                  glm::mat4(1.0f), {size.x, size.y, 1.0f});

        s_Data->TextureShader->SetMat4("u_Transform", transform);

        s_Data->QuadVertexArray->Bind();
        glDrawElements(GL_TRIANGLES, s_Data->QuadVertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
    }

    // Ротований варіант
    void Renderer2D::DrawRotatedQuad(const glm::vec2 &position, const glm::vec2 &size, float rotation,
                                     const glm::vec4 &color) {
        DrawRotatedQuad(position, size, rotation, s_Data->WhiteTexture, color);
    }

    void Renderer2D::DrawRotatedQuad(const glm::vec2 &position, const glm::vec2 &size, float rotation,
                                     const std::shared_ptr<Graphics::Textures> &texture, const glm::vec4 &tintColor) {
        s_Data->TextureShader->SetFloat4("u_Color", tintColor);
        texture->Bind(0);

        // Додаємо поворот (Rotation) навколо осі Z
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), {position.x, position.y, 0.0f}) * glm::rotate(
                                  glm::mat4(1.0f), glm::radians(rotation), {0.0f, 0.0f, 1.0f}) *
                              glm::scale(glm::mat4(1.0f), {size.x, size.y, 1.0f});

        s_Data->TextureShader->SetMat4("u_Transform", transform);

        s_Data->QuadVertexArray->Bind();
        glDrawElements(GL_TRIANGLES, s_Data->QuadVertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
    }
}
