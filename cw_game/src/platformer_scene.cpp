#include "render_command.h"
#include "game_components.h"
#include "platformer_scene.h"

#include <cmath>
#include <cstdlib>
#include <algorithm>
#include "engine.h"


using json = nlohmann::json;
using namespace Engine::Core;
using namespace Engine::Scene;
using namespace Engine::Graphics;

PlatformerScene::PlatformerScene() : Scene("Platformer") {
    GetCamera().SetProjection(-16.0f, 16.0f, -9.0f, 9.0f);
    SpawnBackground();
    LoadLevelFromJson("res/scene_file/main_scene.json");
    SpawnPlayer();
    Input::Get().Subscribe("Exit", [this]() { m_ShouldQuit = true; });
}

void PlatformerScene::Update(float dt)  {
    if (m_ShouldQuit) {
        Application::Get().Quit();
        return;
    }
    if (m_GameWon) return;

    float time = static_cast<float>(Time::GetTime());
    UpdateParallax(dt);
    UpdateCoins(time);

    UpdatePlayer(dt);
    UpdatePhysics(dt);
    UpdateCollisions();

    UpdateHierarchy();
    UpdateCamera(dt);

    Scene::Update(dt);
}

void PlatformerScene::OnRender()  {
    RenderCommand::SetClearColor({0.04f, 0.02f, 0.08f, 1.0f});
    RenderCommand::Clear();
    Scene::OnRender();
}

Entity PlatformerScene::GetPlayer() const { return m_Player; }
bool PlatformerScene::IsGameWon() const { return m_GameWon; }

void PlatformerScene::Restart() {
    m_GameWon = false;

    if (GetRegistry().Has<TransformComponent>(m_Player)) {
        auto &t = GetRegistry().Get<TransformComponent>(m_Player);
        auto &rb = GetRegistry().Get<RigidbodyComponent>(m_Player);
        auto &pc = GetRegistry().Get<PlayerComponent>(m_Player);

        t.Position = {0.0f, -2.0f, 0.2f};
        rb.Velocity = {0.0f, 0.0f};

        // Скидаємо стани гравця
        pc.HasDash = true;
        pc.IsDashing = false;
        pc.IsWallSliding = false;
        pc.TouchingWall = false;
        pc.OnGround = false;

        pc.CoinsCollected = 0;
        GetRegistry().View<CoinComponent, SpriteRendererComponent>(
            [](EntityID, CoinComponent &c, SpriteRendererComponent &s) {
                c.Collected = false;
                s.Visible = true;
            });
    }
}

void PlatformerScene::AttachChild(Entity parent, Entity child) {
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

void PlatformerScene::LoadLevelFromJson(const std::string &filepath) {
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
        col.Size = {(float) p["w"], (float) p["h"]};
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

void PlatformerScene::SpawnPlayer() {
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

void PlatformerScene::SpawnBackground() {
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

void PlatformerScene::UpdateHierarchy() {
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
                    t.Position = {(t.Position.x < 0 ? -0.12f : 0.12f), 0.15f, 0.1f};
                }
            }
        });
    updateChildren(m_Player, updateChildren);
}

void PlatformerScene::UpdateParallax(float dt) {
    if (!GetRegistry().Has<PlayerComponent>(m_Player)) return;
    auto &pt = GetRegistry().Get<TransformComponent>(m_Player);
    GetRegistry().View<TransformComponent, ParallaxLayerComponent>(
        [camX = pt.Position.x, camY = pt.Position.y](EntityID, TransformComponent &t, ParallaxLayerComponent &p) {
            t.Position.x = p.OriginalX - camX * p.ScrollSpeed;
            t.Position.y = 5.0f - camY * (p.ScrollSpeed * 0.5f);
        });
}

void PlatformerScene::UpdateCoins(float time) {
    GetRegistry().View<TransformComponent, SpriteRendererComponent, CoinComponent>(
        [time](EntityID, TransformComponent &t, SpriteRendererComponent &s, CoinComponent &c) {
            if (c.Collected) {
                s.Visible = false;
                return;
            }
            float pulse = std::sin(time * c.Speed + c.Offset) * 0.04f;
            t.Scale = {0.3f + pulse, 0.3f + pulse};
            s.Color = {1.0f, 0.85f + std::sin(time * c.Speed * 1.3f + c.Offset) * 0.15f, 0.2f, 1.0f};
            t.Position.y += std::sin(time * c.Speed + c.Offset) * 0.00001f;
        });
}

void PlatformerScene::UpdatePlayer(float dt) {
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
                if (rb.Velocity.y <= -3.0f) rb.Velocity.y = -3.0f;
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

    if (!pc.OnGround && !pc.IsWallSliding) {
        float stretch = 1.0f + std::abs(rb.Velocity.y) * 0.015f;
        t.Scale = {0.5f / stretch, 0.7f * stretch};
    } else if (pc.IsWallSliding) {
        t.Scale = {0.4f, 0.8f};
    } else {
        t.Scale = {0.5f, 0.7f};
        if (std::abs(rb.Velocity.x) < 0.1f) t.Scale = {0.55f, 0.65f};
    }

    if (t.Position.y<=-15.0f) {
        t.Position = {0.0f, -2.0f, 0.2f};
        rb.Velocity = {0.0f, 0.0f};
    }
}

void PlatformerScene::UpdatePhysics(float dt) {
    GetRegistry().View<TransformComponent, RigidbodyComponent>(
        [dt](auto entity, TransformComponent &t, RigidbodyComponent &rb) {
            if (rb.IsStatic) return;

            if (rb.UseGravity) {
                rb.Velocity.y -= 35.0f * dt;
            }

            t.Position.x += rb.Velocity.x * dt;
            t.Position.y += rb.Velocity.y * dt;
        });
}

void PlatformerScene::UpdateCollisions() {
    if (!GetRegistry().Has<PlayerComponent>(m_Player)) return;

    auto &pt = GetRegistry().Get<TransformComponent>(m_Player);
    auto &prb = GetRegistry().Get<RigidbodyComponent>(m_Player);
    auto &pcol = GetRegistry().Get<ColliderComponent>(m_Player);
    auto &ppc = GetRegistry().Get<PlayerComponent>(m_Player);

    ppc.OnGround = false;
    ppc.TouchingWall = false;

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
                    pt.Position.y = cMax.y + pcol.Size.y * 0.5f;
                    if (prb.Velocity.y < 0) prb.Velocity.y = 0;
                    ppc.OnGround = true;
                } else if (minOverlap == overlapY_bottom) {
                    pt.Position.y = cMin.y - pcol.Size.y * 0.5f;
                    if (prb.Velocity.y > 0) prb.Velocity.y = 0;
                } else if (minOverlap == overlapX_left) {
                    pt.Position.x = cMin.x - pcol.Size.x * 0.5f;
                    prb.Velocity.x = 0;
                    ppc.TouchingWall = true;
                    ppc.WallDir = 1.0f;
                } else {
                    pt.Position.x = cMax.x + pcol.Size.x * 0.5f;
                    prb.Velocity.x = 0;
                    ppc.TouchingWall = true;
                    ppc.WallDir = -1.0f;
                }
            }
        });

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

void PlatformerScene::UpdateCamera(float dt) {
    if (!GetRegistry().Has<PlayerComponent>(m_Player)) return;
    auto &pt = GetRegistry().Get<TransformComponent>(m_Player);
    glm::vec3 camPos = GetCamera().GetPosition();

    // Камера трішки запізнюється, створюючи "кінематографічний" ефект (Lerp)
    glm::vec3 target = {pt.Position.x * 0.3f, pt.Position.y + 2.0f, 0.0f};
    glm::vec3 newPos = camPos + (target - camPos) * 6.0f * dt;

    newPos.x = std::clamp(newPos.x, -6.0f, 6.0f);
    GetCamera().SetPosition(newPos);
}
