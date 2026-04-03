#include "engine.h"
#include "Renderer2D.h"
#include "render_command.h"
#include "scene.h"
#include "scene_manager.h"
#include "components.h"

#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <json.hpp> // Підключаємо парсер JSON
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <algorithm>

using json = nlohmann::json;
using namespace Engine::Core;
using namespace Engine::Scene;
using namespace Engine::Graphics;

// ── Кастомні компоненти ───────────────────────────────────────────────────────

struct PlatformComponent {
    float Width = 2.0f;
    float Height = 0.3f;
};

struct CoinComponent {
    float Offset = 0.0f;
    float Speed = 2.0f;
    bool Collected = false;
};

struct PlayerComponent {
    bool OnGround = false;
    float JumpForce = 15.0f;
    float MoveSpeed = 10.0f;
    int CoinsCollected = 0;
};

struct ParallaxLayerComponent {
    float ScrollSpeed = 0.5f;
    float OriginalX = 0.0f;
};


// ── Сцена платформера ─────────────────────────────────────────────────────────

class PlatformerScene : public Scene {
public:
    PlatformerScene() : Scene("Platformer") {
        GetCamera().SetProjection(-16.0f, 16.0f, -9.0f, 9.0f);

        SpawnBackground();
        LoadLevelFromJson("res/scene_file/main_scene.json"); // Завантажуємо рівень з файлу!
        SpawnPlayer();

        Input::Get().Subscribe("Exit", [this]() { m_ShouldQuit = true; });
    }

    void Update(float dt) override {
        if (m_ShouldQuit) {
            Application::Get().Quit();
            return;
        }

        float time = static_cast<float>(glfwGetTime());

        UpdateParallax(dt);
        UpdateCoins(time);
        UpdatePlayer(dt);
        UpdatePhysics(dt);
        UpdateCollisions();

        UpdateHierarchy(); // Оновлюємо позиції дочірніх об'єктів!

        UpdateCamera(dt);

        Scene::Update(dt);
    }

    void  OnRender() override {
        RenderCommand::SetClearColor({0.03f, 0.03f, 0.10f, 1.0f}); // Темніший фон для неону
        RenderCommand::Clear();
        Scene::OnRender();
    }

    Entity GetPlayer() const { return m_Player; }

private:
    Entity m_Player = {NULL_ENTITY};
    bool m_ShouldQuit = false;

    // Хелпер для створення ієрархії
    void AttachChild(Entity parent, Entity child) {
        // Отримуємо сирі ID сутностей (якщо Entity це клас, можливо доведеться використати (uint32_t)parent)
        uint32_t parentID = (uint32_t) parent;
        uint32_t childID = (uint32_t) child;

        if (!GetRegistry().Has<RelationshipComponent>(parent)) GetRegistry().Add<RelationshipComponent>(parent);
        if (!GetRegistry().Has<RelationshipComponent>(child)) GetRegistry().Add<RelationshipComponent>(child);

        auto &pRel = GetRegistry().Get<RelationshipComponent>(parent);
        auto &cRel = GetRegistry().Get<RelationshipComponent>(child);

        cRel.Parent = parentID;

        if (pRel.FirstChild == 0) {
            pRel.FirstChild = childID;
        } else {
            uint32_t current = pRel.FirstChild;
            // Проходимо по списку братів/сестер, поки Entity(current) існує
            while (GetRegistry().Get<RelationshipComponent>(Entity(current)).NextSibling != 0) {
                current = GetRegistry().Get<RelationshipComponent>(Entity(current)).NextSibling;
            }
            GetRegistry().Get<RelationshipComponent>(Entity(current)).NextSibling = childID;
        }
    }

    // ── Завантаження з JSON ───────────────────────────────────────────────────
    void LoadLevelFromJson(const std::string &filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            std::cerr << "Не вдалося завантажити карту: " << filepath << "\n";
            return;
        }

        json levelData;
        file >> levelData;

        // 1. Завантажуємо платформи
        if (levelData.contains("Platforms")) {
            for (const auto &p: levelData["Platforms"]) {
                Entity e = CreateEntity("Platform");
                auto &t = GetRegistry().Get<TransformComponent>(e);
                t.Position = {p["x"], p["y"], 0.0f};
                t.Scale = {p["w"], 0.4f};

                auto &s = GetRegistry().Add<SpriteRendererComponent>(e);
                s.Color = {p["color"][0], p["color"][1], p["color"][2], p["color"][3]};
                s.ZLayer = 1;

                auto &pc = GetRegistry().Add<PlatformComponent>(e);
                pc.Width = p["w"];
                pc.Height = 0.4f;

                auto &col = GetRegistry().Add<ColliderComponent>(e);
                col.Size = {p["w"], 0.4f};

                auto &rb = GetRegistry().Add<RigidbodyComponent>(e);
                rb.IsStatic = true;
            }
        }

        // 2. Завантажуємо монети
        if (levelData.contains("Coins")) {
            float phase = 0.0f;
            for (const auto &coin: levelData["Coins"]) {
                Entity e = CreateEntity("Coin");
                auto &t = GetRegistry().Get<TransformComponent>(e);
                t.Position = {coin["x"], coin["y"], 0.1f};
                t.Scale = {0.25f, 0.25f};

                auto &s = GetRegistry().Add<SpriteRendererComponent>(e);
                s.Color = {1.0f, 0.85f, 0.0f, 1.0f};
                s.ZLayer = 3;

                auto &c = GetRegistry().Add<CoinComponent>(e);
                c.Offset = phase;
                c.Speed = 2.0f + (phase * 0.3f);
                phase += 0.4f;

                auto &col = GetRegistry().Add<ColliderComponent>(e);
                col.Size = {0.25f, 0.25f};
                col.IsTrigger = true;
            }
        }
    }

    void SpawnPlayer() {
        m_Player = CreateEntity("Player");

        auto &t = GetRegistry().Get<TransformComponent>(m_Player);
        t.Position = {-8.0f, 0.0f, 0.2f};
        t.Scale = {0.5f, 0.7f};

        auto &s = GetRegistry().Add<SpriteRendererComponent>(m_Player);
        s.Color = {0.2f, 0.6f, 1.0f, 1.0f};
        s.ZLayer = 5;

        auto &rb = GetRegistry().Add<RigidbodyComponent>(m_Player);
        rb.IsStatic = false;
        rb.UseGravity = true;
        rb.Drag = 2.0f;

        auto &col = GetRegistry().Add<ColliderComponent>(m_Player);
        col.Size = {0.48f, 0.68f};

        GetRegistry().Add<PlayerComponent>(m_Player);

        // --- ІЄРАРХІЯ ОЧЕЙ ---
        for (int i = 0; i < 2; ++i) {
            Entity eye = CreateEntity("Eye");
            auto &et = GetRegistry().Get<TransformComponent>(eye);

            // Локальна позиція ВІДНОСНО гравця (центрирована по 0,0)
            float offsetX = (i == 0 ? -0.12f : 0.12f);
            et.Position = {offsetX, 0.15f, 0.1f}; // Z = 0.1 над гравцем
            et.Scale = {0.1f, 0.1f}; // Очі трохи більші

            auto &es = GetRegistry().Add<SpriteRendererComponent>(eye);
            es.Color = {1.0f, 1.0f, 1.0f, 1.0f};
            es.ZLayer = 6;

            // Прив'язуємо око до гравця
            AttachChild(m_Player, eye);
        }
    }

    void SpawnBackground() {
        // [Залишаємо ваш старий код спавну фону для паралаксу]
        struct LayerDesc {
            glm::vec4 Color;
            float Z;
            float Speed;
            float W;
            float H;
            float Y;
        };
        LayerDesc layers[] = {
            {{0.05f, 0.05f, 0.15f, 1.0f}, -0.9f, 0.05f, 40.0f, 20.0f, 0.0f},
            {{0.08f, 0.15f, 0.25f, 1.0f}, -0.8f, 0.15f, 6.0f, 12.0f, -2.0f}
        };

        for (auto &l: layers) {
            for (int i = -2; i <= 2; ++i) {
                Entity e = CreateEntity("BG");
                auto &t = GetRegistry().Get<TransformComponent>(e);
                t.Position = {i * l.W, l.Y, l.Z};
                t.Scale = {l.W, l.H};
                auto &s = GetRegistry().Add<SpriteRendererComponent>(e);
                s.Color = l.Color;
                auto &p = GetRegistry().Add<ParallaxLayerComponent>(e);
                p.ScrollSpeed = l.Speed;
                p.OriginalX = t.Position.x;
            }
        }
    }

    // ── Системи оновлення ──────────────────────────────────────────────────────

    // НОВА СИСТЕМА: Оновлює позиції дітей відносно батьків
    void UpdateHierarchy() {
        auto updateChildren = [&](uint32_t parentID, auto &self) -> void {
            Entity parentEntity = Entity(parentID);
            if (!GetRegistry().Has<RelationshipComponent>(parentEntity)) return;

            auto &pRel = GetRegistry().Get<RelationshipComponent>(parentEntity);
            auto &pTransform = GetRegistry().Get<TransformComponent>(parentEntity);

            uint32_t currentChild = pRel.FirstChild;
            while (currentChild != 0) {
                Entity childEntity = Entity(currentChild);
                if (GetRegistry().Has<TransformComponent>(childEntity)) {
                    auto &cTransform = GetRegistry().Get<TransformComponent>(childEntity);

                    cTransform.Position.x = pTransform.Position.x + (cTransform.Position.x * pTransform.Scale.x);
                    cTransform.Position.y = pTransform.Position.y + (cTransform.Position.y * pTransform.Scale.y);
                }

                self(currentChild, self);
                currentChild = GetRegistry().Get<RelationshipComponent>(childEntity).NextSibling;
            }
        };

        // Скидаємо локальні координати очей перед перерахунком (бо вони міняються)
        // Для повноцінної роботи потрібен кеш локальних координат в TransformComponent,
        // але для цього прикладу просто перепризначаємо їх:
        GetRegistry().View<RelationshipComponent, TransformComponent>(
            [&](EntityID e, RelationshipComponent &r, TransformComponent &t) {
                if (r.Parent != NULL_ENTITY && GetRegistry().Has<TagComponent>(e)) {
                    if (GetRegistry().Get<TagComponent>(e).Name == "Eye") {
                        t.Position = {(t.Position.x < 0 ? -0.12f : 0.12f), 0.15f, 0.1f};
                    }
                }
            });

        // Запускаємо від гравця
        updateChildren(m_Player, updateChildren);
    }

    void UpdateParallax(float dt) {
        if (!GetRegistry().Has<PlayerComponent>(m_Player)) return;
        auto &pt = GetRegistry().Get<TransformComponent>(m_Player);
        GetRegistry().View<TransformComponent, ParallaxLayerComponent>(
            [camX = pt.Position.x](EntityID, TransformComponent &t, ParallaxLayerComponent &p) {
                t.Position.x = p.OriginalX - camX * p.ScrollSpeed;
            });
    }

    void UpdateCoins(float time) {
        GetRegistry().View<TransformComponent, SpriteRendererComponent, CoinComponent>(
            [time](EntityID, TransformComponent &t, SpriteRendererComponent &s, CoinComponent &c) {
                if (c.Collected) {
                    s.Visible = false;
                    return;
                }
                float pulse = std::sin(time * c.Speed + c.Offset) * 0.04f;
                t.Scale = {0.25f + pulse, 0.25f + pulse};
                float bright = 0.85f + std::sin(time * c.Speed * 1.3f + c.Offset) * 0.15f;
                s.Color = {1.0f, bright, 0.2f, 1.0f}; // Більш золотий колір
                t.Position.y += std::sin(time * c.Speed + c.Offset) * 0.0015f;
            });
    }

    void UpdatePlayer(float dt) {
        if (!GetRegistry().Has<PlayerComponent>(m_Player)) return;
        auto &rb = GetRegistry().Get<RigidbodyComponent>(m_Player);
        auto &pc = GetRegistry().Get<PlayerComponent>(m_Player);
        auto &t = GetRegistry().Get<TransformComponent>(m_Player);
        auto &s = GetRegistry().Get<SpriteRendererComponent>(m_Player);
        auto &input = Input::Get();

        glm::vec2 dir = {0.0f, 0.0f};
        if (input.IsKeyHeld(KeyCode::A) || input.IsKeyHeld(KeyCode::Left)) dir.x -= 1.0f;
        if (input.IsKeyHeld(KeyCode::D) || input.IsKeyHeld(KeyCode::Right)) dir.x += 1.0f;

        rb.Velocity.x = dir.x * pc.MoveSpeed;

        if ((input.IsKeyHeld(KeyCode::Space) || input.IsKeyHeld(KeyCode::W)) && pc.OnGround) {
            rb.Velocity.y = pc.JumpForce;
            pc.OnGround = false;
        }

        // Анімація сквашу/стретчу
        if (!pc.OnGround) {
            float stretch = 1.0f + std::abs(rb.Velocity.y) * 0.015f;
            t.Scale = {0.5f / stretch, 0.7f * stretch};
        } else {
            t.Scale = {0.5f, 0.7f};
            // Приземлення робить гравця трохи ширшим (squash)
            if (std::abs(rb.Velocity.x) < 0.1f) t.Scale = {0.55f, 0.65f};
        }

        if (t.Position.y < -15.0f) {
            t.Position = {-8.0f, -1.0f, 0.2f};
            rb.Velocity = {0.0f, 0.0f};
        }
    }

    // [UpdatePhysics, UpdateCollisions та UpdateCamera залишаються без змін]
    // Для стислості я їх приховав, але ви використовуєте свій попередній код для них.
   void UpdatePhysics(float dt) {
        GetRegistry().View<TransformComponent, RigidbodyComponent>(
            [dt](auto entity, TransformComponent& t, RigidbodyComponent& rb) {

                if (rb.IsStatic) return;

                if (rb.UseGravity) {
                    rb.Velocity.y -= 20.0f * dt;
                }

                rb.Velocity.x *= (1.0f - rb.Drag * dt * 0.5f);

                t.Position.x += rb.Velocity.x * dt;
                t.Position.y += rb.Velocity.y * dt;
            });
    }

    void UpdateCollisions() {
        if (!GetRegistry().Has<PlayerComponent>(m_Player)) return;

        auto& pt   = GetRegistry().Get<TransformComponent>(m_Player);
        auto& prb  = GetRegistry().Get<RigidbodyComponent>(m_Player);
        auto& pcol = GetRegistry().Get<ColliderComponent>(m_Player);
        auto& ppc  = GetRegistry().Get<PlayerComponent>(m_Player);

        ppc.OnGround = false;

        // Гравець vs Платформи
        GetRegistry().View<TransformComponent, ColliderComponent, PlatformComponent>(
            [&](auto entity, TransformComponent& t, ColliderComponent& col, PlatformComponent&) {
                glm::vec2 pMin = { pt.Position.x - pcol.Size.x * 0.5f, pt.Position.y - pcol.Size.y * 0.5f };
                glm::vec2 pMax = { pt.Position.x + pcol.Size.x * 0.5f, pt.Position.y + pcol.Size.y * 0.5f };

                glm::vec2 cMin = { t.Position.x - col.Size.x * 0.5f, t.Position.y - col.Size.y * 0.5f };
                glm::vec2 cMax = { t.Position.x + col.Size.x * 0.5f, t.Position.y + col.Size.y * 0.5f };

                if (pMax.x > cMin.x && pMin.x < cMax.x &&
                    pMax.y > cMin.y && pMin.y < cMax.y) {

                    float overlapY_top    = cMax.y - pMin.y;
                    float overlapY_bottom = pMax.y - cMin.y;
                    float overlapX_left   = pMax.x - cMin.x;
                    float overlapX_right  = cMax.x - pMin.x;

                    float minOverlap = std::min({overlapY_top, overlapY_bottom, overlapX_left, overlapX_right});

                    if (minOverlap == overlapY_top) {
                        pt.Position.y = cMax.y + pcol.Size.y * 0.5f;
                        if (prb.Velocity.y < 0) prb.Velocity.y = 0;
                        ppc.OnGround = true;
                    } else if (minOverlap == overlapY_bottom) {
                        pt.Position.y = cMin.y - pcol.Size.y * 0.5f;
                        if (prb.Velocity.y > 0) prb.Velocity.y = 0;
                    } else if (minOverlap == overlapX_left) {
                        pt.Position.x = cMin.x - pcol.Size.x * 0.5f;
                        prb.Velocity.x = 0;
                    } else {
                        pt.Position.x = cMax.x + pcol.Size.x * 0.5f;
                        prb.Velocity.x = 0;
                    }
                }
            });

        // Гравець vs Монети
        GetRegistry().View<TransformComponent, ColliderComponent, CoinComponent>(
            [&](auto entity, TransformComponent& t, ColliderComponent& col, CoinComponent& c) {
                if (c.Collected) return;

                glm::vec2 pMin = { pt.Position.x - pcol.Size.x * 0.5f, pt.Position.y - pcol.Size.y * 0.5f };
                glm::vec2 pMax = { pt.Position.x + pcol.Size.x * 0.5f, pt.Position.y + pcol.Size.y * 0.5f };

                glm::vec2 cMin = { t.Position.x - col.Size.x * 0.5f, t.Position.y - col.Size.y * 0.5f };
                glm::vec2 cMax = { t.Position.x + col.Size.x * 0.5f, t.Position.y + col.Size.y * 0.5f };

                if (pMax.x > cMin.x && pMin.x < cMax.x &&
                    pMax.y > cMin.y && pMin.y < cMax.y) {
                    c.Collected = true;
                    ppc.CoinsCollected++;
                }
            });
    }

    void UpdateCamera(float dt) {
        if (!GetRegistry().Has<PlayerComponent>(m_Player)) return;
        auto& pt = GetRegistry().Get<TransformComponent>(m_Player);

        glm::vec3 camPos  = GetCamera().GetPosition();
        glm::vec3 target  = { pt.Position.x, pt.Position.y + 1.5f, 0.0f };
        glm::vec3 newPos  = camPos + (target - camPos) * 5.0f * dt;

        // Обмеження камери
        newPos.x = std::clamp(newPos.x, -8.0f, 8.0f);
        newPos.y = std::clamp(newPos.y, -4.0f, 6.0f);
        GetCamera().SetPosition(newPos);
    }
};

// ── Application ───────────────────────────────────────────────────────────────
// [Залишається без змін, як у вашому оригіналі]
class GameApp : public Engine::Core::Application {
public:
    GameApp() {
        RenderCommand::Init();
        Renderer2D::Init();

        auto& input = Input::Get();
        input.BindAction("Exit", KeyCode::Escape);

        auto scene = std::make_shared<PlatformerScene>();
        SceneManager::Add("Platformer", scene);
        SceneManager::LoadScene("Platformer");
    }

    ~GameApp() override {
        Renderer2D::Shutdown();
        SceneManager::Clear();
    }

    // ТУТ МАЄ БУТИ Update (без On)
    void Update(float dt) override {
        if (auto* scene = SceneManager::GetActive())
            scene->Update(dt); // А у сцени викликаємо OnUpdate
    }

    // ТУТ МАЄ БУТИ Render (без On)
    void Render() override {
        if (auto* scene = SceneManager::GetActive())
            scene->OnRender(); // А у сцени викликаємо OnRender

        auto stats = Renderer2D::GetStats();
        auto* pc = dynamic_cast<PlatformerScene*>(SceneManager::GetActive());

        int coins = 0;
        if (pc) {
            auto& reg = pc->GetRegistry();
            if (reg.Has<PlayerComponent>(pc->GetPlayer())) {
                coins = reg.Get<PlayerComponent>(pc->GetPlayer()).CoinsCollected;
            }
        }

        std::ostringstream title;
        title << "CW Platformer  |  "
              << "DrawCalls: " << stats.DrawCalls
              << "  Quads: "   << stats.QuadCount
              << "  Coins: "   << coins << "/" << 60
              << "  FPS: " << Time::GetFPS();

        glfwSetWindowTitle(GetWindow().GetNativeWindow(), title.str().c_str());
        Renderer2D::ResetStats();
    }
};

Engine::Core::Application* Engine::Core::CreateApplication() {
    return new GameApp();
}