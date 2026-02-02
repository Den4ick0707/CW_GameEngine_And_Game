#include "GameApp.h"
#include "PlayerController.h"

#include <EngineCore/Input.h>
#include <Graphics/Renderer2D.h>
#include <Scene/GameObject.h>
#include <Scene/SpriteRendener.h>
#include <Physics/BoxCollider2D.h>
#include <Physics/RigidBody2D.h>

// Робимо імена з просторів імен рушія доступними в цьому файлі
using namespace Engine::Scene;
using namespace Engine::Physics;
using namespace Engine::Graphics;

// Функція-хелпер для створення платформ, щоб не дублювати код
void GameApp::CreatePlatform(const std::string& name, const glm::vec3& position, const glm::vec3& scale, const glm::vec4& color) {
    auto platform = m_Scene->CreateGameObject(name);
    platform->Position = position;
    platform->Scale = scale;
    platform->AddComponent<SpriteRenderer>(color);
    platform->AddComponent<Engine::BoxCollider2D>(glm::vec2(1.0f, 1.0f));
}

GameApp::GameApp() : m_Camera(-16.0f, 16.0f, -9.0f, 9.0f) {
    m_Scene = std::make_unique<Scene>();

    // --- ГРАВЕЦЬ (Синій) ---
    auto player = m_Scene->CreateGameObject("Player");
    player->Position = {-14.0f, -7.0f, 0.0f};
    player->AddComponent<SpriteRenderer>(glm::vec4(0.2f, 0.4f, 0.8f, 1.0f));
    player->AddComponent<Engine::BoxCollider2D>(glm::vec2(1.0f, 1.0f));
    player->AddComponent<RigidBody2D>();
    player->AddComponent<PlayerController>();

    // --- БУДУЄМО РІВЕНЬ ---
    CreatePlatform("Ground", {0.0f, -8.5f, -0.5f}, {32.0f, 1.0f, 1.0f}, {0.2f, 0.6f, 0.3f, 1.0f});
    CreatePlatform("LeftWall", {-16.5f, 0.0f, 0.0f}, {1.0f, 18.0f, 1.0f}, {0.2f, 0.2f, 0.2f, 1.0f});
    CreatePlatform("RightWall", {16.5f, 0.0f, 0.0f}, {1.0f, 18.0f, 1.0f}, {0.2f, 0.2f, 0.2f, 1.0f});
    CreatePlatform("Platform1", {-8.0f, -5.0f, 0.0f}, {4.0f, 0.5f, 1.0f}, {0.6f, 0.4f, 0.2f, 1.0f});
    CreatePlatform("Platform2", {0.0f, -2.0f, 0.0f}, {5.0f, 0.5f, 1.0f}, {0.6f, 0.4f, 0.2f, 1.0f});
    CreatePlatform("Platform3", {8.0f, 1.0f, 0.0f}, {3.0f, 0.5f, 1.0f}, {0.6f, 0.4f, 0.2f, 1.0f});
    CreatePlatform("SecretPlatform", {0.0f, 6.0f, 0.0f}, {2.0f, 0.5f, 1.0f}, {0.8f, 0.8f, 0.2f, 1.0f});
}

GameApp::~GameApp() {
}

void GameApp::OnUpdate(float dt) {
    m_Scene->OnUpdate(dt);
}

void GameApp::OnRender() {
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Повертаємо префікс Engine::, оскільки Renderer2D знаходиться в просторі імен Engine
    Engine::Renderer2D::BeginScene(m_Camera);
    m_Scene->OnRender();
    Engine::Renderer2D::EndScene();
}
