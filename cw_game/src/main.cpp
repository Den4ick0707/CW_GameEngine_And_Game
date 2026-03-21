#include "engine.h"
#include "Renderer2D.h"
#include "render_command.h"
#include "scene.h"
#include "scene_manager.h"
#include "components.h"

#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <sstream>
#include <vector>
#include <cmath>
#include <cstdlib>

using namespace Engine::Core;
using namespace Engine::Scene;
using namespace Engine::Graphics;

// ── Кастомні компоненти ───────────────────────────────────────────────────────

struct PlatformComponent {
    float Width  = 2.0f;
    float Height = 0.3f;
};

struct CoinComponent {
    float Offset   = 0.0f;   // фазовий зсув анімації
    float Speed    = 2.0f;
    bool  Collected = false;
};

struct PlayerComponent {
    bool  OnGround    = false;
    float JumpForce   = 8.0f;
    float MoveSpeed   = 5.0f;
    int   CoinsCollected = 0;
};

struct ParallaxLayerComponent {
    float ScrollSpeed = 0.5f;
    float OriginalX   = 0.0f;
};

// ── Сцена платформера ─────────────────────────────────────────────────────────

class PlatformerScene : public Scene {
public:
    PlatformerScene() : Scene("Platformer") {
            GetCamera().SetProjection(-16.0f, 16.0f, -9.0f, 9.0f);

            SpawnBackground();
            SpawnPlatforms();
            SpawnCoins();
            SpawnPlayer();

            Input::Get().Subscribe("Exit", [this]() { m_ShouldQuit = true; });

    }
    void OnUpdate(float dt) override {
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
        UpdateCamera(dt);

        Scene::OnUpdate(dt);
    }

    void OnRender() override {
        RenderCommand::SetClearColor({ 0.05f, 0.05f, 0.15f, 1.0f });
        RenderCommand::Clear();
        Scene::OnRender();
    }

public:
    Entity GetPlayer() const { return m_Player; }
private:
Entity m_Player = { NULL_ENTITY };
    bool   m_ShouldQuit = false;

    // ── Спавн ─────────────────────────────────────────────────────────────────

    void SpawnBackground() {
        // 3 паралакс шари — великі кольорові прямокутники
        struct LayerDesc { glm::vec4 Color; float Z; float Speed; float W; float H; float Y; };
        LayerDesc layers[] = {
            {{ 0.10f, 0.10f, 0.30f, 1.0f }, -0.9f, 0.05f, 40.0f, 20.0f,  0.0f },
            {{ 0.08f, 0.15f, 0.25f, 1.0f }, -0.8f, 0.15f,  6.0f, 12.0f, -2.0f },
            {{ 0.06f, 0.20f, 0.20f, 1.0f }, -0.7f, 0.30f,  3.0f,  8.0f, -3.0f },
        };

        for (auto& l : layers) {
            // Для паралаксу ставимо кілька копій щоб не було діри
            for (int i = -2; i <= 2; ++i) {
                Entity e = CreateEntity("BG");
                auto& t = GetRegistry().Get<TransformComponent>(e);
                t.Position = { i * l.W, l.Y, l.Z };
                t.Scale    = { l.W, l.H };

                auto& s = GetRegistry().Add<SpriteRendererComponent>(e);
                s.Color  = l.Color;

                auto& p = GetRegistry().Add<ParallaxLayerComponent>(e);
                p.ScrollSpeed = l.Speed;
                p.OriginalX   = t.Position.x;
            }
        }

        // "Зірки" — багато маленьких квадратів (демонстрація batch rendering)
        srand(42);
        for (int i = 0; i < 300; ++i) {
            Entity e = CreateEntity("Star");
            auto& t = GetRegistry().Get<TransformComponent>(e);
            float x = ((rand() % 6400) / 100.0f) - 32.0f;
            float y = ((rand() % 1800) / 100.0f) - 5.0f;
            t.Position = { x, y, -0.6f };
            float sz   = 0.03f + (rand() % 10) / 200.0f;
            t.Scale    = { sz, sz };

            auto& s = GetRegistry().Add<SpriteRendererComponent>(e);
            float bright = 0.5f + (rand() % 50) / 100.0f;
            s.Color = { bright, bright, bright, 1.0f };
            s.ZLayer = -5;
        }
    }

    void SpawnPlatforms() {
        struct PlatDesc { float X, Y, W; glm::vec4 Color; };
        PlatDesc platforms[] = {
            {  0.0f, -5.0f, 30.0f, { 0.20f, 0.60f, 0.20f, 1.0f } }, // земля
            { -8.0f, -2.0f,  4.0f, { 0.30f, 0.50f, 0.30f, 1.0f } },
            { -2.0f, -0.5f,  3.5f, { 0.35f, 0.55f, 0.25f, 1.0f } },
            {  4.0f,  1.0f,  3.0f, { 0.25f, 0.50f, 0.35f, 1.0f } },
            {  9.0f,  2.5f,  2.5f, { 0.20f, 0.45f, 0.40f, 1.0f } },
            { -5.0f,  1.5f,  2.0f, { 0.30f, 0.55f, 0.30f, 1.0f } },
            {  1.0f,  3.5f,  2.0f, { 0.25f, 0.60f, 0.25f, 1.0f } },
            {  7.0f,  5.0f,  3.0f, { 0.20f, 0.50f, 0.35f, 1.0f } },
            { -10.0f, 0.5f,  2.5f, { 0.35f, 0.45f, 0.30f, 1.0f } },
        };

        for (auto& p : platforms) {
            Entity e = CreateEntity("Platform");
            auto& t = GetRegistry().Get<TransformComponent>(e);
            t.Position = { p.X, p.Y, 0.0f };
            t.Scale    = { p.W, 0.4f };

            auto& s = GetRegistry().Add<SpriteRendererComponent>(e);
            s.Color  = p.Color;
            s.ZLayer = 1;

            auto& pc = GetRegistry().Add<PlatformComponent>(e);
            pc.Width  = p.W;
            pc.Height = 0.4f;

            auto& col = GetRegistry().Add<ColliderComponent>(e);
            col.Size   = { p.W, 0.4f };
            col.Offset = { 0.0f, 0.0f };

            auto& rb = GetRegistry().Add<RigidbodyComponent>(e);
            rb.IsStatic = true;

            // Декоративна смужка зверху платформи
            Entity top = CreateEntity("PlatformTop");
            auto& tt = GetRegistry().Get<TransformComponent>(top);
            tt.Position = { p.X, p.Y + 0.22f, 0.0f };
            tt.Scale    = { p.W, 0.04f };
            auto& ts = GetRegistry().Add<SpriteRendererComponent>(top);
            ts.Color  = { p.Color.r + 0.1f, p.Color.g + 0.1f, p.Color.b + 0.1f, 1.0f };
            ts.ZLayer = 2;
        }
    }

    void SpawnCoins() {
        // Монети — 60 штук (демонстрація batch rendering)
        struct CoinPos { float X, Y; };
        std::vector<CoinPos> positions;

        // Навколо платформ
        positions.push_back({ -8.0f, -1.3f });
        positions.push_back({ -7.0f, -1.3f });
        positions.push_back({ -6.0f, -1.3f });
        positions.push_back({ -2.0f,  0.2f });
        positions.push_back({ -1.0f,  0.2f });
        positions.push_back({  0.0f,  0.2f });
        positions.push_back({  4.0f,  1.7f });
        positions.push_back({  5.0f,  1.7f });
        positions.push_back({  9.0f,  3.2f });
        positions.push_back({ 10.0f,  3.2f });
        positions.push_back({ -5.0f,  2.2f });
        positions.push_back({  1.0f,  4.2f });
        positions.push_back({  7.0f,  5.7f });
        positions.push_back({  8.0f,  5.7f });

        // Додаткові монети для batch demo
        for (int i = 0; i < 46; ++i) {
            float x = -12.0f + i * 0.55f;
            float y = -4.4f;
            positions.push_back({ x, y });
        }

        float phase = 0.0f;
        for (auto& pos : positions) {
            Entity e = CreateEntity("Coin");
            auto& t = GetRegistry().Get<TransformComponent>(e);
            t.Position = { pos.X, pos.Y, 0.1f };
            t.Scale    = { 0.25f, 0.25f };

            auto& s = GetRegistry().Add<SpriteRendererComponent>(e);
            s.Color  = { 1.0f, 0.85f, 0.0f, 1.0f };
            s.ZLayer = 3;

            auto& c = GetRegistry().Add<CoinComponent>(e);
            c.Offset = phase;
            c.Speed  = 2.0f + (phase * 0.3f);
            phase += 0.4f;

            auto& col = GetRegistry().Add<ColliderComponent>(e);
            col.Size      = { 0.25f, 0.25f };
            col.IsTrigger = true;
        }
    }

    void SpawnPlayer() {
        m_Player = CreateEntity("Player");

        auto& t = GetRegistry().Get<TransformComponent>(m_Player);
        t.Position = { -8.0f, -1.0f, 0.2f };
        t.Scale    = { 0.5f, 0.7f };

        auto& s = GetRegistry().Add<SpriteRendererComponent>(m_Player);
        s.Color  = { 0.2f, 0.6f, 1.0f, 1.0f };
        s.ZLayer = 5;

        auto& rb = GetRegistry().Add<RigidbodyComponent>(m_Player);
        rb.UseGravity = true;
        rb.Drag       = 2.0f;
        rb.Mass       = 1.0f;

        auto& col = GetRegistry().Add<ColliderComponent>(m_Player);
        col.Size = { 0.48f, 0.68f };

        GetRegistry().Add<PlayerComponent>(m_Player);

        // "Очі" гравця
        for (int i = 0; i < 2; ++i) {
            Entity eye = CreateEntity("Eye");
            auto& et = GetRegistry().Get<TransformComponent>(eye);
            et.Position = { t.Position.x + (i == 0 ? -0.1f : 0.1f), t.Position.y + 0.15f, 0.3f };
            et.Scale    = { 0.08f, 0.08f };
            auto& es = GetRegistry().Add<SpriteRendererComponent>(eye);
            es.Color  = { 1.0f, 1.0f, 1.0f, 1.0f };
            es.ZLayer = 6;
        }
    }

    // ── Системи оновлення ──────────────────────────────────────────────────────

    void UpdateParallax(float dt) {
        if (!GetRegistry().Has<PlayerComponent>(m_Player)) return;
        auto& pt = GetRegistry().Get<TransformComponent>(m_Player);
        float camX = pt.Position.x;

        GetRegistry().View<TransformComponent, ParallaxLayerComponent>(
            [camX](EntityID, TransformComponent& t, ParallaxLayerComponent& p) {
                t.Position.x = p.OriginalX - camX * p.ScrollSpeed;
            });
    }

    void UpdateCoins(float time) {
        GetRegistry().View<TransformComponent, SpriteRendererComponent, CoinComponent>(
            [time](EntityID, TransformComponent& t, SpriteRendererComponent& s, CoinComponent& c) {
                if (c.Collected) {
                    s.Visible = false;
                    return;
                }
                // Пульсація розміру
                float pulse = std::sin(time * c.Speed + c.Offset) * 0.04f;
                t.Scale = { 0.25f + pulse, 0.25f + pulse };
                // Пульсація кольору (золото → жовто-білий)
                float bright = 0.85f + std::sin(time * c.Speed * 1.3f + c.Offset) * 0.15f;
                s.Color = { 1.0f, bright, 0.0f, 1.0f };
                // Легке підскакування
                t.Position.y += std::sin(time * c.Speed + c.Offset) * 0.0015f;
            });
    }

    void UpdatePlayer(float dt) {
        if (!GetRegistry().Has<PlayerComponent>(m_Player)) return;

        auto& rb  = GetRegistry().Get<RigidbodyComponent>(m_Player);
        auto& pc  = GetRegistry().Get<PlayerComponent>(m_Player);
        auto& t   = GetRegistry().Get<TransformComponent>(m_Player);
        auto& s   = GetRegistry().Get<SpriteRendererComponent>(m_Player);
        auto& input = Input::Get();

        glm::vec2 dir = { 0.0f, 0.0f };
        if (input.IsKeyHeld(KeyCode::A) || input.IsKeyHeld(KeyCode::Left))  dir.x -= 1.0f;
        if (input.IsKeyHeld(KeyCode::D) || input.IsKeyHeld(KeyCode::Right)) dir.x += 1.0f;

        rb.Velocity.x = dir.x * pc.MoveSpeed;

        // Стрибок
        if ((input.IsKeyHeld(KeyCode::Space) || input.IsKeyHeld(KeyCode::W) ||
             input.IsKeyHeld(KeyCode::Up)) && pc.OnGround) {
            rb.Velocity.y = pc.JumpForce;
            pc.OnGround = false;
        }

        // Squash & Stretch анімація
        if (!pc.OnGround) {
            float stretch = 1.0f + std::abs(rb.Velocity.y) * 0.02f;
            t.Scale = { 0.5f / stretch, 0.7f * stretch };
        } else {
            t.Scale = { 0.5f, 0.7f };
        }

        // Колір гравця залежно від швидкості
        float speed = glm::length(rb.Velocity);
        s.Color = {
            0.2f + speed * 0.03f,
            0.6f - speed * 0.01f,
            1.0f,
            1.0f
        };

        // Ресет якщо впав
        if (t.Position.y < -15.0f) {
            t.Position = { -8.0f, -1.0f, 0.2f };
            rb.Velocity = { 0.0f, 0.0f };
        }
    }

    void UpdatePhysics(float dt) {
        GetRegistry().View<TransformComponent, RigidbodyComponent, ActiveComponent>(
            [dt](EntityID, TransformComponent& t, RigidbodyComponent& rb, ActiveComponent& a) {
                if (!a.Active || rb.IsStatic) return;
                if (rb.UseGravity) rb.Velocity.y -= 20.0f * dt;
                rb.Velocity.x *= (1.0f - rb.Drag * dt * 0.5f);
                t.Position.x += rb.Velocity.x * dt;
                t.Position.y += rb.Velocity.y * dt;
            });
    }

    void UpdateCollisions() {
        if (!GetRegistry().Has<PlayerComponent>(m_Player)) return;

        auto& pt  = GetRegistry().Get<TransformComponent>(m_Player);
        auto& prb = GetRegistry().Get<RigidbodyComponent>(m_Player);
        auto& pcol = GetRegistry().Get<ColliderComponent>(m_Player);
        auto& ppc  = GetRegistry().Get<PlayerComponent>(m_Player);

        ppc.OnGround = false;

        // Гравець vs Платформи
        GetRegistry().View<TransformComponent, ColliderComponent, PlatformComponent>(
            [&](EntityID, TransformComponent& t, ColliderComponent& col, PlatformComponent&) {
                glm::vec2 pMin = { pt.Position.x - pcol.Size.x * 0.5f,
                                   pt.Position.y - pcol.Size.y * 0.5f };
                glm::vec2 pMax = { pt.Position.x + pcol.Size.x * 0.5f,
                                   pt.Position.y + pcol.Size.y * 0.5f };
                glm::vec2 cMin = { t.Position.x - col.Size.x * 0.5f,
                                   t.Position.y - col.Size.y * 0.5f };
                glm::vec2 cMax = { t.Position.x + col.Size.x * 0.5f,
                                   t.Position.y + col.Size.y * 0.5f };

                if (pMax.x > cMin.x && pMin.x < cMax.x &&
                    pMax.y > cMin.y && pMin.y < cMax.y) {

                    float overlapY_top    = cMax.y - pMin.y;
                    float overlapY_bottom = pMax.y - cMin.y;
                    float overlapX_left   = pMax.x - cMin.x;
                    float overlapX_right  = cMax.x - pMin.x;

                    float minOverlap = std::min({overlapY_top, overlapY_bottom,
                                                  overlapX_left, overlapX_right});

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
            [&](EntityID, TransformComponent& t, ColliderComponent& col, CoinComponent& c) {
                if (c.Collected) return;
                glm::vec2 pMin = { pt.Position.x - pcol.Size.x * 0.5f,
                                   pt.Position.y - pcol.Size.y * 0.5f };
                glm::vec2 pMax = { pt.Position.x + pcol.Size.x * 0.5f,
                                   pt.Position.y + pcol.Size.y * 0.5f };
                glm::vec2 cMin = { t.Position.x - col.Size.x * 0.5f,
                                   t.Position.y - col.Size.y * 0.5f };
                glm::vec2 cMax = { t.Position.x + col.Size.x * 0.5f,
                                   t.Position.y + col.Size.y * 0.5f };

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
        newPos.x = glm::clamp(newPos.x, -8.0f, 8.0f);
        newPos.y = glm::clamp(newPos.y, -4.0f, 6.0f);
        GetCamera().SetPosition(newPos);
    }
};

// ── Application ───────────────────────────────────────────────────────────────

class GameApp : public Application {
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

    ~GameApp() {
        Renderer2D::Shutdown();
        SceneManager::Clear();
    }

    void Update(float dt) override {
        if (auto* scene = SceneManager::GetActive())
            scene->OnUpdate(dt);
    }

    void Render() override {
        if (auto* scene = SceneManager::GetActive())
            scene->OnRender();

        auto stats = Renderer2D::GetStats();
        auto* pc = dynamic_cast<PlatformerScene*>(SceneManager::GetActive());
        int coins = 0;
        if (pc) {
            auto& reg = pc->GetRegistry();
            if (reg.Has<PlayerComponent>(pc->GetPlayer()))
                coins = reg.Get<PlayerComponent>(pc->GetPlayer()).CoinsCollected;
        }

        std::ostringstream title;
        title << "CW Platformer  |  "
              << "DrawCalls: " << stats.DrawCalls
              << "  Quads: "   << stats.QuadCount
              << "  Coins: "   << coins
              << "/" << 60;
        glfwSetWindowTitle(GetWindow().GetNativeWindow(), title.str().c_str());
        Renderer2D::ResetStats();
    }
};

Application* Engine::Core::CreateApplication() {
    return new GameApp();
}