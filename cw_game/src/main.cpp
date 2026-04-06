#include "engine.h"
#include "Renderer2D.h"
#include "render_command.h"
#include "scene.h"
#include "scene_manager.h"
#include "components.h"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <json.hpp>
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

// ── Ігрові компоненти ───────────────────────────────────────────────────────
struct PlatformComponent {
    float Width = 2.0f;
    float Height = 0.3f;
};

struct CoinComponent {
    float Offset = 0.0f;
    float Speed = 2.0f;
    bool Collected = false;
};

struct ParallaxLayerComponent {
    float ScrollSpeed = 0.5f;
    float OriginalX = 0.0f;
};

struct SpikeComponent {
    bool Lethal = true;
};

struct BouncerComponent {
    float BounceForce = 28.0f;
};

struct WinZoneComponent {
    bool Reached = false;
};

struct PlayerComponent {
    bool OnGround = false;
    float MoveSpeed = 12.0f;
    float JumpForce = 20.0f;
    int CoinsCollected = 0;

    // Стан кнопок
    bool SpaceWasPressed = false;
    bool DashWasPressed = false;

    // --- CELESTE / HOLLOW KNIGHT МЕХАНІКИ ---
    // Ривок (Dash)
    bool HasDash = true;
    bool IsDashing = false;
    float DashTimer = 0.0f;
    float DashDuration = 0.15f; // Тривалість ривка (в секундах)
    float DashSpeed = 30.0f;
    float FacingDir = 1.0f; // 1 (вправо) або -1 (вліво)

    // Стіни
    bool TouchingWall = false;
    float WallDir = 0.0f; // З якого боку стіна (-1 або 1)
    bool IsWallSliding = false;
};

// ── Сцена гри ─────────────────────────────────────────────────────────

class PlatformerScene : public Scene {
public:
    PlatformerScene() : Scene("Platformer") {
        GetCamera().SetProjection(-16.0f, 16.0f, -9.0f, 9.0f);
        SpawnBackground();
        LoadLevelFromJson("res/scene_file/main_scene.json");
        SpawnPlayer();
        Input::Get().Subscribe("Exit", [this]() { m_ShouldQuit = true; });
    }

    void Update(float dt) override {
        if (m_ShouldQuit) {
            Application::Get().Quit();
            return;
        }
        if (m_GameWon) return;

        float time = static_cast<float>(glfwGetTime());
        UpdateParallax(dt);
        UpdateCoins(time);

        // Порядок важливий: Спочатку фізика гравця, потім колізії, потім перерахунок
        UpdatePlayer(dt);
        UpdatePhysics(dt);
        UpdateCollisions();

        UpdateHierarchy();
        UpdateCamera(dt);

        Scene::Update(dt);
    }

    void OnRender() override {
        RenderCommand::SetClearColor({0.04f, 0.02f, 0.08f, 1.0f}); // Глибокий фіолетовий фон
        RenderCommand::Clear();
        Scene::OnRender();
    }

    Entity GetPlayer() const { return m_Player; }
    bool IsGameWon() const { return m_GameWon; }

private:
    Entity m_Player = {NULL_ENTITY};
    bool m_ShouldQuit = false;
    bool m_GameWon = false;

    void AttachChild(Entity parent, Entity child) {
        uint32_t parentID = (uint32_t) parent;
        uint32_t childID = (uint32_t) child;
        if (!GetRegistry().Has<RelationshipComponent>(parent)) GetRegistry().Add<RelationshipComponent>(parent);
        if (!GetRegistry().Has<RelationshipComponent>(child)) GetRegistry().Add<RelationshipComponent>(child);
        auto &pRel = GetRegistry().Get<RelationshipComponent>(parent);
        auto &cRel = GetRegistry().Get<RelationshipComponent>(child);
        cRel.Parent = parentID;
        if (pRel.FirstChild == 0) { pRel.FirstChild = childID; } else {
            uint32_t current = pRel.FirstChild;
            while (GetRegistry().Get<RelationshipComponent>(Entity(current)).NextSibling != 0) {
                current = GetRegistry().Get<RelationshipComponent>(Entity(current)).NextSibling;
            }
            GetRegistry().Get<RelationshipComponent>(Entity(current)).NextSibling = childID;
        }
    }

    void LoadLevelFromJson(const std::string &filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) return;
        json levelData;
        file >> levelData;

        auto loadRects = [&](const char *key, auto setupFunc) {
            if (levelData.contains(key)) {
                for (const auto &p: levelData[key]) {
                    Entity e = CreateEntity(key);
                    auto &t = GetRegistry().Get<TransformComponent>(e);
                    float w = p["w"];
                    float h = p.contains("h") ? (float) p["h"] : 0.4f;
                    t.Position = {p["x"], p["y"], 0.0f};
                    t.Scale = {w, h};
                    auto &s = GetRegistry().Add<SpriteRendererComponent>(e);
                    if (p.contains("color")) s.Color = {p["color"][0], p["color"][1], p["color"][2], p["color"][3]};
                    auto &col = GetRegistry().Add<ColliderComponent>(e);
                    col.Size = {w, h};
                    setupFunc(e, col, s, p);
                }
            }
        };

        loadRects("Platforms", [&](Entity e, ColliderComponent &col, SpriteRendererComponent &s, const json &p) {
            GetRegistry().Add<PlatformComponent>(e);
            GetRegistry().Add<RigidbodyComponent>(e).IsStatic = true;
        });

        loadRects("Spikes", [&](Entity e, ColliderComponent &col, SpriteRendererComponent &s, const json &p) {
            s.Color = {0.9f, 0.1f, 0.2f, 1.0f};
            s.ZLayer = 2;
            col.Size = {(float) p["w"] * 0.7f, (float) p["h"] * 0.7f};
            col.IsTrigger = true;
            GetRegistry().Add<SpikeComponent>(e);
        });

        loadRects("Bouncers", [&](Entity e, ColliderComponent &col, SpriteRendererComponent &s, const json &p) {
            s.Color = {1.0f, 0.8f, 0.1f, 1.0f};
            col.IsTrigger = true;
            GetRegistry().Add<BouncerComponent>(e);
        });

        loadRects("FinishLine", [&](Entity e, ColliderComponent &col, SpriteRendererComponent &s, const json &p) {
            s.Color = {0.2f, 1.0f, 0.5f, 0.6f};
            col.IsTrigger = true;
            GetRegistry().Add<WinZoneComponent>(e);
            // Отримуємо Transform напряму з реєстру
            GetRegistry().Get<TransformComponent>(e).Position.z = -0.1f;
        });

        if (levelData.contains("Coins")) {
            float phase = 0.0f;
            for (const auto &coin: levelData["Coins"]) {
                Entity e = CreateEntity("Coin");
                auto &t = GetRegistry().Get<TransformComponent>(e);
                t.Position = {coin["x"], coin["y"], 0.1f};
                t.Scale = {0.3f, 0.3f};
                auto &s = GetRegistry().Add<SpriteRendererComponent>(e);
                s.Color = {1.0f, 0.8f, 0.0f, 1.0f};
                s.ZLayer = 3;
                auto &c = GetRegistry().Add<CoinComponent>(e);
                c.Offset = phase;
                c.Speed = 2.0f + (phase * 0.3f);
                phase += 0.4f;
                auto &col = GetRegistry().Add<ColliderComponent>(e);
                col.Size = {0.3f, 0.3f};
                col.IsTrigger = true;
            }
        }
    }

    void SpawnPlayer() {
        m_Player = CreateEntity("Player");
        auto &t = GetRegistry().Get<TransformComponent>(m_Player);
        t.Position = {0.0f, -2.0f, 0.2f};
        t.Scale = {0.5f, 0.7f};
        auto &s = GetRegistry().Add<SpriteRendererComponent>(m_Player);
        s.Color = {0.2f, 0.9f, 1.0f, 1.0f};
        s.ZLayer = 5; // Світло-блакитний (Cyan)
        auto &rb = GetRegistry().Add<RigidbodyComponent>(m_Player);
        rb.IsStatic = false;
        rb.UseGravity = true;
        rb.Drag = 2.0f;
        auto &col = GetRegistry().Add<ColliderComponent>(m_Player);
        col.Size = {0.45f, 0.65f}; // Хітбокс трохи менший за візуал
        GetRegistry().Add<PlayerComponent>(m_Player);

        for (int i = 0; i < 2; ++i) {
            Entity eye = CreateEntity("Eye");
            auto &et = GetRegistry().Get<TransformComponent>(eye);
            et.Position = {(i == 0 ? -0.12f : 0.12f), 0.15f, 0.1f};
            et.Scale = {0.1f, 0.1f};
            GetRegistry().Add<SpriteRendererComponent>(eye).Color = {1.0f, 1.0f, 1.0f, 1.0f};
            GetRegistry().Get<SpriteRendererComponent>(eye).ZLayer = 6;
            AttachChild(m_Player, eye);
        }
    }

    void SpawnBackground() {
        struct LayerDesc {
            glm::vec4 Color;
            float Z, Speed, W, H, Y;
        };
        LayerDesc layers[] = {
            {{0.02f, 0.02f, 0.08f, 1.0f}, -0.9f, 0.05f, 40.0f, 60.0f, 10.0f},
            {{0.05f, 0.10f, 0.15f, 1.0f}, -0.8f, 0.15f, 15.0f, 40.0f, 5.0f}
        };
        for (auto &l: layers) {
            for (int i = -2; i <= 2; ++i) {
                Entity e = CreateEntity("BG");
                auto &t = GetRegistry().Get<TransformComponent>(e);
                t.Position = {i * l.W, l.Y, l.Z};
                t.Scale = {l.W, l.H};
                GetRegistry().Add<SpriteRendererComponent>(e).Color = l.Color;
                auto &p = GetRegistry().Add<ParallaxLayerComponent>(e);
                p.ScrollSpeed = l.Speed;
                p.OriginalX = t.Position.x;
            }
        }
    }

    void UpdateHierarchy() {
        auto updateChildren = [&](uint32_t parentID, auto &self) -> void {
            Entity parentEntity = Entity(parentID);
            if (!GetRegistry().Has<RelationshipComponent>(parentEntity)) return;
            auto &pRel = GetRegistry().Get<RelationshipComponent>(parentEntity);
            auto &pTransform = GetRegistry().Get<TransformComponent>(parentEntity);

            float parentDir = 1.0f; // Напрям батька для очей
            if (GetRegistry().Has<PlayerComponent>(parentEntity)) {
                parentDir = GetRegistry().Get<PlayerComponent>(parentEntity).FacingDir;
            }

            uint32_t currentChild = pRel.FirstChild;
            while (currentChild != 0) {
                Entity childEntity = Entity(currentChild);
                if (GetRegistry().Has<TransformComponent>(childEntity)) {
                    auto &cTransform = GetRegistry().Get<TransformComponent>(childEntity);

                    // Якщо це очі, зміщуємо їх в бік руху
                    float xOffset = cTransform.Position.x * pTransform.Scale.x;
                    if (GetRegistry().Has<TagComponent>(childEntity) && GetRegistry().Get<TagComponent>(childEntity).
                        Name == "Eye") {
                        xOffset += parentDir * 0.08f;
                    }

                    cTransform.Position.x = pTransform.Position.x + xOffset;
                    cTransform.Position.y = pTransform.Position.y + (cTransform.Position.y * pTransform.Scale.y);
                }
                self(currentChild, self);
                currentChild = GetRegistry().Get<RelationshipComponent>(childEntity).NextSibling;
            }
        };

        GetRegistry().View<RelationshipComponent, TransformComponent>(
            [&](EntityID e, RelationshipComponent &r, TransformComponent &t) {
                if (r.Parent != NULL_ENTITY && GetRegistry().Has<TagComponent>(e)) {
                    if (GetRegistry().Get<TagComponent>(e).Name == "Eye") {
                        // Скидаємо базову позицію очей
                        t.Position = {(t.Position.x < 0 ? -0.12f : 0.12f), 0.15f, 0.1f};
                    }
                }
            });
        updateChildren(m_Player, updateChildren);
    }

    void UpdateParallax(float dt) {
        if (!GetRegistry().Has<PlayerComponent>(m_Player)) return;
        auto &pt = GetRegistry().Get<TransformComponent>(m_Player);
        GetRegistry().View<TransformComponent, ParallaxLayerComponent>(
            [camX = pt.Position.x, camY = pt.Position.y](EntityID, TransformComponent &t, ParallaxLayerComponent &p) {
                t.Position.x = p.OriginalX - camX * p.ScrollSpeed;
                // Легкий вертикальний паралакс
                t.Position.y = 5.0f - camY * (p.ScrollSpeed * 0.5f);
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
                t.Scale = {0.3f + pulse, 0.3f + pulse};
                s.Color = {1.0f, 0.85f + std::sin(time * c.Speed * 1.3f + c.Offset) * 0.15f, 0.2f, 1.0f};
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

        bool left = input.IsKeyHeld(KeyCode::A) || input.IsKeyHeld(KeyCode::Left);
        bool right = input.IsKeyHeld(KeyCode::D) || input.IsKeyHeld(KeyCode::Right);
        bool space = input.IsKeyHeld(KeyCode::Space) || input.IsKeyHeld(KeyCode::W);
        bool dashInput = input.IsKeyHeld(KeyCode::C) || input.IsKeyHeld(KeyCode::LeftShift); // Підтримка кількох кнопок

        float dirX = 0.0f;
        if (left) dirX -= 1.0f;
        if (right) dirX += 1.0f;

        if (dirX != 0.0f) pc.FacingDir = dirX; // Запам'ятовуємо, куди дивиться гравець

        // --- 1. ЛОГІКА РИВКА (DASH) ---
        if (dashInput && !pc.DashWasPressed && pc.HasDash && !pc.IsDashing) {
            pc.IsDashing = true;
            pc.DashTimer = pc.DashDuration;
            pc.HasDash = false;
            // Ривок робиться в напрямку натискання, або туди, куди дивимось
            float dX = dirX != 0.0f ? dirX : pc.FacingDir;
            rb.Velocity = {dX * pc.DashSpeed, 0.0f}; // Ігноруємо вертикальну швидкість
        }
        pc.DashWasPressed = dashInput;

        if (pc.IsDashing) {
            pc.DashTimer -= dt;
            rb.UseGravity = false; // Вимикаємо гравітацію під час ривка
            s.Color = {1.0f, 1.0f, 1.0f, 1.0f}; // Спалах білим

            if (pc.DashTimer <= 0.0f) {
                pc.IsDashing = false;
                rb.UseGravity = true;
                rb.Velocity.x *= 0.5f; // Втрата імпульсу після ривка
            }
        } else {
            rb.UseGravity = true;

            // Відновлення кольору
            if (pc.HasDash) s.Color = {0.2f, 0.9f, 1.0f, 1.0f}; // Є ривок - блакитний
            else s.Color = {0.1f, 0.4f, 0.8f, 1.0f}; // Немає ривка - темно синій

            // --- 2. НОРМАЛЬНИЙ РУХ ТА СТІНИ ---
            rb.Velocity.x = dirX * pc.MoveSpeed;

            // Відновлення ривка на землі або на стіні
            if (pc.OnGround || pc.TouchingWall) {
                pc.HasDash = true;
            }

            // Ковзання по стіні
            pc.IsWallSliding = false;
            if (pc.TouchingWall && !pc.OnGround && rb.Velocity.y < 0.0f) {
                // Якщо гравець тисне в бік стіни
                if ((pc.WallDir == -1.0f && left) || (pc.WallDir == 1.0f && right)) {
                    pc.IsWallSliding = true;
                    // Обмежуємо швидкість падіння (ковзання)
                    if (rb.Velocity.y < -3.0f) rb.Velocity.y = -3.0f;
                }
            }

            // --- 3. СТРИБКИ ---
            if (space && !pc.SpaceWasPressed) {
                if (pc.OnGround) {
                    // Звичайний стрибок
                    rb.Velocity.y = pc.JumpForce;
                    pc.OnGround = false;
                } else if (pc.TouchingWall && !pc.OnGround) {
                    // Стрибок ВІД стіни (Wall Jump)
                    rb.Velocity.y = pc.JumpForce * 0.9f;
                    rb.Velocity.x = pc.WallDir * -pc.MoveSpeed * 1.5f; // Відштовхуємось в протилежний бік
                    pc.FacingDir = -pc.WallDir;
                }
            }

            // ЗМІННА ВИСОТА СТРИБКА (Відпускання пробілу зрізає швидкість)
            if (!space && pc.SpaceWasPressed && rb.Velocity.y > 0.0f && !pc.TouchingWall) {
                rb.Velocity.y *= 0.4f;
            }
        }
        pc.SpaceWasPressed = space;

        // Анімація Squash/Stretch
        if (!pc.OnGround && !pc.IsWallSliding) {
            float stretch = 1.0f + std::abs(rb.Velocity.y) * 0.015f;
            t.Scale = {0.5f / stretch, 0.7f * stretch};
        } else if (pc.IsWallSliding) {
            t.Scale = {0.4f, 0.8f}; // Витягуємось по вертикалі при ковзанні
        } else {
            t.Scale = {0.5f, 0.7f};
            if (std::abs(rb.Velocity.x) < 0.1f) t.Scale = {0.55f, 0.65f};
        }

        // Падіння за карту
        if (t.Position.y < -15.0f) {
            t.Position = {0.0f, -2.0f, 0.2f};
            rb.Velocity = {0.0f, 0.0f};
        }
    }

    void UpdatePhysics(float dt) {
        GetRegistry().View<TransformComponent, RigidbodyComponent>(
            [dt](auto entity, TransformComponent &t, RigidbodyComponent &rb) {
                if (rb.IsStatic) return;

                if (rb.UseGravity) {
                    rb.Velocity.y -= 35.0f * dt; // Збільшена гравітація (як у Celeste) для різкості
                }

                t.Position.x += rb.Velocity.x * dt;
                t.Position.y += rb.Velocity.y * dt;
            });
    }

    void UpdateCollisions() {
        if (!GetRegistry().Has<PlayerComponent>(m_Player)) return;

        auto &pt = GetRegistry().Get<TransformComponent>(m_Player);
        auto &prb = GetRegistry().Get<RigidbodyComponent>(m_Player);
        auto &pcol = GetRegistry().Get<ColliderComponent>(m_Player);
        auto &ppc = GetRegistry().Get<PlayerComponent>(m_Player);

        ppc.OnGround = false;
        ppc.TouchingWall = false;

        // 1. Платформи (і Стіни)
        GetRegistry().View<TransformComponent, ColliderComponent, PlatformComponent>(
            [&](auto entity, TransformComponent &t, ColliderComponent &col, PlatformComponent &) {
                glm::vec2 pMin = {pt.Position.x - pcol.Size.x * 0.5f, pt.Position.y - pcol.Size.y * 0.5f};
                glm::vec2 pMax = {pt.Position.x + pcol.Size.x * 0.5f, pt.Position.y + pcol.Size.y * 0.5f};
                glm::vec2 cMin = {t.Position.x - col.Size.x * 0.5f, t.Position.y - col.Size.y * 0.5f};
                glm::vec2 cMax = {t.Position.x + col.Size.x * 0.5f, t.Position.y + col.Size.y * 0.5f};

                if (pMax.x > cMin.x && pMin.x < cMax.x && pMax.y > cMin.y && pMin.y < cMax.y) {
                    float overlapY_top = cMax.y - pMin.y;
                    float overlapY_bottom = pMax.y - cMin.y;
                    float overlapX_left = pMax.x - cMin.x;
                    float overlapX_right = cMax.x - pMin.x;
                    float minOverlap = std::min({overlapY_top, overlapY_bottom, overlapX_left, overlapX_right});

                    if (minOverlap == overlapY_top) {
                        // Стали на платформу
                        pt.Position.y = cMax.y + pcol.Size.y * 0.5f;
                        if (prb.Velocity.y < 0) prb.Velocity.y = 0;
                        ppc.OnGround = true;
                    } else if (minOverlap == overlapY_bottom) {
                        // Вдарились головою
                        pt.Position.y = cMin.y - pcol.Size.y * 0.5f;
                        if (prb.Velocity.y > 0) prb.Velocity.y = 0;
                    } else if (minOverlap == overlapX_left) {
                        // Стіна справа
                        pt.Position.x = cMin.x - pcol.Size.x * 0.5f;
                        prb.Velocity.x = 0;
                        ppc.TouchingWall = true;
                        ppc.WallDir = 1.0f;
                    } else {
                        // Стіна зліва
                        pt.Position.x = cMax.x + pcol.Size.x * 0.5f;
                        prb.Velocity.x = 0;
                        ppc.TouchingWall = true;
                        ppc.WallDir = -1.0f;
                    }
                }
            });

        // 2. Шипи (Смерть)
        bool hitSpike = false;
        GetRegistry().View<TransformComponent, ColliderComponent, SpikeComponent>(
            [&](auto entity, TransformComponent &t, ColliderComponent &col, SpikeComponent &) {
                glm::vec2 pMin = {pt.Position.x - pcol.Size.x * 0.5f, pt.Position.y - pcol.Size.y * 0.5f};
                glm::vec2 pMax = {pt.Position.x + pcol.Size.x * 0.5f, pt.Position.y + pcol.Size.y * 0.5f};
                glm::vec2 cMin = {t.Position.x - col.Size.x * 0.5f, t.Position.y - col.Size.y * 0.5f};
                glm::vec2 cMax = {t.Position.x + col.Size.x * 0.5f, t.Position.y + col.Size.y * 0.5f};
                if (pMax.x > cMin.x && pMin.x < cMax.x && pMax.y > cMin.y && pMin.y < cMax.y) hitSpike = true;
            });
        if (hitSpike) {
            pt.Position = {0.0f, -2.0f, 0.2f};
            prb.Velocity = {0.0f, 0.0f};
        }

        // 3. Батути
        GetRegistry().View<TransformComponent, ColliderComponent, BouncerComponent>(
            [&](auto entity, TransformComponent &t, ColliderComponent &col, BouncerComponent &b) {
                glm::vec2 pMin = {pt.Position.x - pcol.Size.x * 0.5f, pt.Position.y - pcol.Size.y * 0.5f};
                glm::vec2 pMax = {pt.Position.x + pcol.Size.x * 0.5f, pt.Position.y + pcol.Size.y * 0.5f};
                glm::vec2 cMin = {t.Position.x - col.Size.x * 0.5f, t.Position.y - col.Size.y * 0.5f};
                glm::vec2 cMax = {t.Position.x + col.Size.x * 0.5f, t.Position.y + col.Size.y * 0.5f};
                if (pMax.x > cMin.x && pMin.x < cMax.x && pMax.y > cMin.y && pMin.y < cMax.y) {
                    prb.Velocity.y = b.BounceForce;
                    ppc.HasDash = true; // Батут повертає ривок!
                    ppc.IsDashing = false; // Зупиняємо ривок, якщо влетіли в батут
                }
            });

        // 4. Монети
        GetRegistry().View<TransformComponent, ColliderComponent, CoinComponent>(
            [&](auto entity, TransformComponent &t, ColliderComponent &col, CoinComponent &c) {
                if (c.Collected) return;
                glm::vec2 pMin = {pt.Position.x - pcol.Size.x * 0.5f, pt.Position.y - pcol.Size.y * 0.5f};
                glm::vec2 pMax = {pt.Position.x + pcol.Size.x * 0.5f, pt.Position.y + pcol.Size.y * 0.5f};
                glm::vec2 cMin = {t.Position.x - col.Size.x * 0.5f, t.Position.y - col.Size.y * 0.5f};
                glm::vec2 cMax = {t.Position.x + col.Size.x * 0.5f, t.Position.y + col.Size.y * 0.5f};
                if (pMax.x > cMin.x && pMin.x < cMax.x && pMax.y > cMin.y && pMin.y < cMax.y) {
                    c.Collected = true;
                    ppc.CoinsCollected++;
                }
            });

        // 5. Фінішна зона
        GetRegistry().View<TransformComponent, ColliderComponent, WinZoneComponent>(
            [&](auto entity, TransformComponent &t, ColliderComponent &col, WinZoneComponent &w) {
                glm::vec2 pMin = {pt.Position.x - pcol.Size.x * 0.5f, pt.Position.y - pcol.Size.y * 0.5f};
                glm::vec2 pMax = {pt.Position.x + pcol.Size.x * 0.5f, pt.Position.y + pcol.Size.y * 0.5f};
                glm::vec2 cMin = {t.Position.x - col.Size.x * 0.5f, t.Position.y - col.Size.y * 0.5f};
                glm::vec2 cMax = {t.Position.x + col.Size.x * 0.5f, t.Position.y + col.Size.y * 0.5f};
                if (pMax.x > cMin.x && pMin.x < cMax.x && pMax.y > cMin.y && pMin.y < cMax.y) {
                    m_GameWon = true;
                }
            });
    }

    void UpdateCamera(float dt) {
        if (!GetRegistry().Has<PlayerComponent>(m_Player)) return;
        auto &pt = GetRegistry().Get<TransformComponent>(m_Player);
        glm::vec3 camPos = GetCamera().GetPosition();

        // Камера трішки запізнюється, створюючи "кінематографічний" ефект (Lerp)
        glm::vec3 target = {pt.Position.x * 0.3f, pt.Position.y + 2.0f, 0.0f};
        glm::vec3 newPos = camPos + (target - camPos) * 6.0f * dt;

        newPos.x = std::clamp(newPos.x, -6.0f, 6.0f);
        GetCamera().SetPosition(newPos);
    }
};

// ── Application ───────────────────────────────────────────────────────────────
class GameApp : public Engine::Core::Application {
public:
    GameApp() {
        RenderCommand::Init();
        Renderer2D::Init();
        Input::Get().BindAction("Exit", KeyCode::Escape);

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        (void) io;
        ImGui::StyleColorsDark();
        GLFWwindow *window = static_cast<GLFWwindow *>(GetWindow().GetNativeWindow());
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 330");

        auto scene = std::make_shared<PlatformerScene>();
        SceneManager::Add("Platformer", scene);
        SceneManager::LoadScene("Platformer");
    }

    ~GameApp() override {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        Renderer2D::Shutdown();
        SceneManager::Clear();
    }

    void Update(float dt) override {
        if (auto *scene = SceneManager::GetActive()) scene->Update(dt);
    }

    void Render() override {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (auto *scene = SceneManager::GetActive()) scene->OnRender();

        auto *pc = dynamic_cast<PlatformerScene *>(SceneManager::GetActive());

        // Панель інструментів
        ImGui::Begin("Celeste-like Debug");
        ImGui::Text("FPS: %.1f", Time::GetFPS());
        ImGui::Text("Controls:");
        ImGui::BulletText("A/D or Arrows: Move");
        ImGui::BulletText("Space/W: Jump (Hold to jump higher)");
        ImGui::BulletText("C or Shift: DASH (Air dash resets on floor/walls)");
        ImGui::Separator();

        if (pc && pc->GetRegistry().Has<PlayerComponent>(pc->GetPlayer())) {
            auto &pComp = pc->GetRegistry().Get<PlayerComponent>(pc->GetPlayer());
            ImGui::Text("Dash Ready: %s", pComp.HasDash ? "YES" : "NO");
            ImGui::Text("Wall Sliding: %s", pComp.IsWallSliding ? "YES" : "NO");
        }

        if (ImGui::Button("Restart Level")) {
            if (pc && pc->GetRegistry().Has<TransformComponent>(pc->GetPlayer())) {
                pc->GetRegistry().Get<TransformComponent>(pc->GetPlayer()).Position = {0.0f, -2.0f, 0.2f};
                pc->GetRegistry().Get<RigidbodyComponent>(pc->GetPlayer()).Velocity = {0.0f, 0.0f};
            }
        }
        ImGui::End();

        // Екран перемоги
        if (pc && pc->IsGameWon()) {
            ImGui::SetNextWindowPos(ImVec2(GetWindow().GetWidth() * 0.5f, GetWindow().GetHeight() * 0.5f),
                                    ImGuiCond_Always, ImVec2(0.5f, 0.5f));
            ImGui::Begin("WIN", nullptr,
                         ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize |
                         ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing |
                         ImGuiWindowFlags_NoNav);
            ImGui::SetWindowFontScale(3.0f);
            ImGui::TextColored(ImVec4(0.2f, 1.0f, 0.8f, 1.0f), "TOWER CONQUERED!");
            ImGui::SetWindowFontScale(1.0f);
            ImGui::Text("You've mastered the Dash and Wall Jumps.");
            ImGui::End();
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        Renderer2D::ResetStats();
    }
};

Engine::Core::Application *Engine::Core::CreateApplication() { return new GameApp(); }
