#include <iostream>
#include <memory>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "window.h" // Твій клас вікна з Engine::Core
#include "buffer_layout.h"
#include "vertex_buffer.h"
#include "index_buffer.h"
#include "vertex_array.h"
#include "shader_module.h"
#include "shader_program.h"
#include "OrthographicCamera.h"
#include "Rendener.h"
#include "render_command.h"

using namespace Engine::Graphics;

int main() {
    // 1. Створюємо вікно (припускаю, що твій клас Window має такий інтерфейс)
    // Якщо у тебе ініціалізація інакша - адаптуй цей рядок.
    Engine::Core::WindowProps props("Мій Перший Рендер!", 1280, 720);

    // Передаємо ці налаштування у вікно
    Engine::Core::Window window(props);

    // Переконайся, що GLAD завантажився (якщо це не робиться всередині Window)
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // 2. Створюємо дані для Квадрата (Координати + Колір)
    // Оскільки ми у 2D, Z завжди 0.0f
    float vertices[] = {
        // Позиція (X, Y, Z)     // Колір (R, G, B, A)
        -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // Лівий нижній (Червоний)
        0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, // Правий нижній (Зелений)
        0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, // Правий верхній (Синій)
        -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f // Лівий верхній (Жовтий)
    };

    // 3. Індекси (Як з'єднати вершини у 2 трикутники)
    uint32_t indices[] = {
        0, 1, 2, // Перший трикутник
        2, 3, 0 // Другий трикутник
    };

    // 4. Налаштовуємо Буфери та VAO
    auto vao = std::make_shared<VertexArray>();

    auto vbo = std::make_shared<VertexBuffer>(vertices, sizeof(vertices));
    vbo->SetLayout({
        {ShaderDataType::Float3, "a_Position"},
        {ShaderDataType::Float4, "a_Color"}
    });
    vao->AddVertexBuffer(vbo);

    auto ibo = std::make_shared<IndexBuffer>(indices, sizeof(indices) / sizeof(uint32_t));
    vao->SetIndexBuffer(ibo);

    // 5. Завантажуємо Шейдери
    auto shaderProgram = std::make_shared<ShaderProgram>();
    ShaderModule vertShader("res/shaders/basic.vert", ShaderType::VERTEX);
    ShaderModule fragShader("res/shaders/basic.frag", ShaderType::FRAGMENT);

    shaderProgram->AttachShader(vertShader);
    shaderProgram->AttachShader(fragShader);
    shaderProgram->Link();

    // 6. Створюємо Камеру (Aspect ratio для 1280x720 = 1.777)
    OrthographicCamera camera(-1.777f, 1.777f, -1.0f, 1.0f);

    // --- ГОЛОВНИЙ ІГРОВИЙ ЦИКЛ ---
    while (!window.ShouldClose()) {
        // Очищаємо екран (темно-сірий колір)
        RenderCommand::SetClearColor({0.15f, 0.15f, 0.15f, 1.0f});
        RenderCommand::Clear();

        // Починаємо сцену
        Renderer::BeginScene(camera);

        // Рухаємо наш квадрат (наприклад, трохи вправо і крутимо)
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));

        // Відправляємо на малювання!
        Renderer::Submit(shaderProgram, vao, transform);

        Renderer::EndScene();

        // Оновлюємо вікно (SwapBuffers + PollEvents)
        // ВАЖЛИВО: Виклич свій метод оновлення вікна, наприклад window.OnUpdate()
        // або glfwSwapBuffers(window.GetNativeWindow()); glfwPollEvents();
        glfwSwapBuffers(window.GetNativeWindow());
        glfwPollEvents();
    }

    return 0;
}
