#include "engine.h"
#include "Renderer2D.h"
#include "render_command.h"
#include "scene.h"
#include "components.h"

#include <imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <json.hpp>
#include <fstream>
#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <algorithm>
#include "game_components.h"
#include "racing_scene.h"


using json = nlohmann::json;

using namespace Engine::Core;
using namespace Engine::Scene;
using namespace Engine::Graphics;


RacingScene::RacingScene()
    : Scene("Racing") {
    GetCamera().SetProjection(-16.0f, 16.0f, -9.0f, 9.0f);

    LoadTrackFromJson("res/scene_file/track_scene.json");
    SpawnCar();

    PrewarmSkidMarks();

    Input::Get().Subscribe("Exit", [this]() { m_ShouldQuit = true; });
}

void RacingScene::Update(float dt)  {
    if (m_ShouldQuit) {
        Application::Get().Quit();
        return;
    }

    UpdateCar(dt);
    UpdateCollisions();
    UpdateSkidMarks(dt);
    UpdateCamera(dt);

    Scene::Update(dt);
}

void RacingScene::OnRender()  {
    RenderCommand::SetClearColor({0.15f, 0.5f, 0.15f, 1.0f});
    RenderCommand::Clear();
    Scene::OnRender();
}

Entity RacingScene::GetCar() const { return m_Car; }

void RacingScene::Restart() {
    if (GetRegistry().Has<TransformComponent>(m_Car)) {
        auto &t = GetRegistry().Get<TransformComponent>(m_Car);
        auto &car = GetRegistry().Get<CarComponent>(m_Car);

        t.Position = {10.0f, -15.0f, 0.2f};
        t.Rotation = 0.0f;

        car.Speed = 0.0f;
        car.CurrentLap = 0;
        car.CurrentLapTime = 0.0f;
        car.PassedHalfway = false;
    }
}

void RacingScene::PrewarmSkidMarks() {
    for (int i = 0; i < 150; ++i) {
        Entity skid = CreateEntity("SkidMark");
        auto &t = GetRegistry().Get<TransformComponent>(skid);
        t.Position = {0.0f, 0.0f, 0.1f};
        t.Scale = {0.4f, 0.4f};

        auto &s = GetRegistry().Add<SpriteRendererComponent>(skid);
        s.Color = {0.1f, 0.1f, 0.1f, 0.6f};
        s.ZLayer = 1;
        s.Visible = false;

        auto &sm = GetRegistry().Add<SkidMarkComponent>(skid);
        sm.LifeTime = -1.0f;
    }
}

void RacingScene::SpawnSkidMark(glm::vec3 pos) {
    bool found = false;
    GetRegistry().View<SkidMarkComponent, TransformComponent, SpriteRendererComponent>(
        [&](EntityID, SkidMarkComponent &skid, TransformComponent &t, SpriteRendererComponent &s) {
            if (!found && skid.LifeTime <= 0.0f) {
                t.Position = {pos.x, pos.y, 0.1f};
                skid.LifeTime = skid.MaxLife;
                s.Visible = true;
                s.Color.w = 0.6f;
                found = true;
            }
        });
}


void RacingScene::LoadTrackFromJson(const std::string &filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Не вдалося завантажити трасу. Створюємо базову генерацію.\n";
        SpawnFallbackTrack();
        return;
    }

    json levelData;
    file >> levelData;

    if (levelData.contains("Walls")) {
        for (const auto &p: levelData["Walls"]) {
            Entity e = CreateEntity("Wall");
            auto &t = GetRegistry().Get<TransformComponent>(e);
            t.Position = {p["x"], p["y"], 0.0f};
            t.Scale = {p["w"], p["h"]};

            auto &s = GetRegistry().Add<SpriteRendererComponent>(e);
            s.Color = {0.8f, 0.1f, 0.1f, 1.0f};
            if (p.contains("color")) s.Color = {p["color"][0], p["color"][1], p["color"][2], p["color"][3]};

            auto &col = GetRegistry().Add<ColliderComponent>(e);
            col.Size = {p["w"], p["h"]};
            GetRegistry().Add<TrackWallComponent>(e);
        }
    }

    if (levelData.contains("Asphalt")) {
        for (const auto &p: levelData["Asphalt"]) {
            Entity e = CreateEntity("Asphalt");
            auto &t = GetRegistry().Get<TransformComponent>(e);
            t.Position = {p["x"], p["y"], -0.5f};
            t.Scale = {p["w"], p["h"]};
            auto &s = GetRegistry().Add<SpriteRendererComponent>(e);
            s.Color = {0.25f, 0.25f, 0.25f, 1.0f};
        }
    }

    if (levelData.contains("Checkpoints")) {
        for (const auto &p: levelData["Checkpoints"]) {
            Entity e = CreateEntity("Checkpoint");
            auto &t = GetRegistry().Get<TransformComponent>(e);
            t.Position = {p["x"], p["y"], -0.4f};
            t.Scale = {p["w"], p["h"]};

            auto &s = GetRegistry().Add<SpriteRendererComponent>(e);
            s.Color = {1.0f, 1.0f, 1.0f, 0.5f};

            auto &col = GetRegistry().Add<ColliderComponent>(e);
            col.Size = {p["w"], p["h"]};
            col.IsTrigger = true;

            auto &cp = GetRegistry().Add<CheckpointComponent>(e);
            cp.IsFinishLine = p.contains("finish") && p["finish"];
            cp.IsHalfwayPoint = p.contains("halfway") && p["halfway"];
        }
    }
}

void RacingScene::SpawnFallbackTrack() {
    struct WallDef {
        float x, y, w, h;
    };
    std::vector<WallDef> walls = {
        {0.0f, 20.0f, 40.0f, 2.0f}, {0.0f, -20.0f, 40.0f, 2.0f},
        {20.0f, 0.0f, 2.0f, 40.0f}, {-20.0f, 0.0f, 2.0f, 40.0f},
        {0.0f, 0.0f, 20.0f, 20.0f}
    };

    for (auto &w: walls) {
        Entity e = CreateEntity("Wall");
        auto &t = GetRegistry().Get<TransformComponent>(e);
        t.Position = {w.x, w.y, 0.0f};
        t.Scale = {w.w, w.h};
        GetRegistry().Add<SpriteRendererComponent>(e).Color = {0.8f, 0.2f, 0.2f, 1.0f};
        GetRegistry().Add<ColliderComponent>(e).Size = {w.w, w.h};
        GetRegistry().Add<TrackWallComponent>(e);
    }

    Entity finish = CreateEntity("Finish");
    GetRegistry().Get<TransformComponent>(finish).Position = {10.0f, -10.0f, -0.4f};
    GetRegistry().Get<TransformComponent>(finish).Scale = {10.0f, 2.0f};
    GetRegistry().Add<SpriteRendererComponent>(finish).Color = {1.0f, 1.0f, 1.0f, 0.4f};
    GetRegistry().Add<ColliderComponent>(finish).Size = {10.0f, 2.0f};
    auto &cp = GetRegistry().Add<CheckpointComponent>(finish);
    cp.IsFinishLine = true;
}

void RacingScene::SpawnCar() {
    m_Car = CreateEntity("Car");
    auto &t = GetRegistry().Get<TransformComponent>(m_Car);
    t.Position = {10.0f, -15.0f, 0.2f};
    t.Scale = {1.8f, 1.0f};

    auto &s = GetRegistry().Add<SpriteRendererComponent>(m_Car);
    s.Color = {0.1f, 0.4f, 0.9f, 1.0f};
    s.ZLayer = 5;

    auto &col = GetRegistry().Add<ColliderComponent>(m_Car);
    col.Size = {1.8f, 1.0f};

    GetRegistry().Add<CarComponent>(m_Car);
}

void RacingScene::UpdateCar(float dt) {
    if (!GetRegistry().Has<CarComponent>(m_Car)) return;
    auto &car = GetRegistry().Get<CarComponent>(m_Car);
    auto &t = GetRegistry().Get<TransformComponent>(m_Car);
    auto &input = Input::Get();

    car.CurrentLapTime += dt;

    bool isAccelerating = input.IsKeyHeld(KeyCode::W) || input.IsKeyHeld(KeyCode::Up);
    bool isBraking = input.IsKeyHeld(KeyCode::S) || input.IsKeyHeld(KeyCode::Down);
    bool left = input.IsKeyHeld(KeyCode::A) || input.IsKeyHeld(KeyCode::Left);
    bool right = input.IsKeyHeld(KeyCode::D) || input.IsKeyHeld(KeyCode::Right);

    if (isAccelerating) {
        car.Speed += car.Acceleration * dt;
    } else if (isBraking) {
        car.Speed -= car.Acceleration * 1.5f * dt;
    } else {
        if (car.Speed > 0.1f) car.Speed -= car.Drag * dt;
        else if (car.Speed < -0.1f) car.Speed += car.Drag * dt;
        else car.Speed = 0.0f;
    }

    car.Speed = std::clamp(car.Speed, -car.MaxSpeed * 0.4f, car.MaxSpeed);

    if (std::abs(car.Speed) > 1.0f) {
        float turnDir = (car.Speed > 0) ? 1.0f : -1.0f;
        float speedRatio = std::min(std::abs(car.Speed) / 10.0f, 1.0f);

        if (left) t.Rotation += car.TurnSpeed * turnDir * speedRatio * dt;
        if (right) t.Rotation -= car.TurnSpeed * turnDir * speedRatio * dt;

        if (t.Rotation >= 360.0f) t.Rotation -= 360.0f;
        if (t.Rotation < 0.0f) t.Rotation += 360.0f;

        if ((left || right) && std::abs(car.Speed) > car.MaxSpeed * 0.7f) {
            SpawnSkidMark(t.Position);
        }
    }

    float radRotation = glm::radians(t.Rotation);
    glm::vec2 forward = {std::cos(radRotation), std::sin(radRotation)};

    t.Position.x += forward.x * car.Speed * dt;
    t.Position.y += forward.y * car.Speed * dt;

    t.CalculateLocalTransform();
}

void RacingScene::UpdateSkidMarks(float dt) {
    GetRegistry().View<SkidMarkComponent, SpriteRendererComponent>(
        [dt](EntityID, SkidMarkComponent &skid, SpriteRendererComponent &s) {
            if (skid.LifeTime > 0.0f) {
                skid.LifeTime -= dt;
                if (skid.LifeTime <= 0.0f) {
                    s.Visible = false;
                } else {
                    s.Color.w = (skid.LifeTime / skid.MaxLife) * 0.6f;
                }
            }
        });
}

void RacingScene::UpdateTransforms() {
    GetRegistry().View<TransformComponent>(
        [](EntityID, TransformComponent &t) {
            glm::mat4 transform = glm::translate(glm::mat4(1.0f), t.Position);

            transform = glm::rotate(transform, t.Rotation, glm::vec3(0.0f, 0.0f, 1.0f));

            transform = glm::scale(transform, glm::vec3(t.Scale.x, t.Scale.y, 1.0f));
        });
}

void RacingScene::UpdateCollisions() {
    if (!GetRegistry().Has<CarComponent>(m_Car)) return;
    auto &carT = GetRegistry().Get<TransformComponent>(m_Car);
    auto &carCol = GetRegistry().Get<ColliderComponent>(m_Car);
    auto &carC = GetRegistry().Get<CarComponent>(m_Car);

    glm::vec2 pMin = {carT.Position.x - carCol.Size.x * 0.5f, carT.Position.y - carCol.Size.y * 0.5f};
    glm::vec2 pMax = {carT.Position.x + carCol.Size.x * 0.5f, carT.Position.y + carCol.Size.y * 0.5f};

    GetRegistry().View<TransformComponent, ColliderComponent, TrackWallComponent>(
        [&](auto entity, TransformComponent &wT, ColliderComponent &wCol, TrackWallComponent &wall) {
            glm::vec2 cMin = {wT.Position.x - wCol.Size.x * 0.5f, wT.Position.y - wCol.Size.y * 0.5f};
            glm::vec2 cMax = {wT.Position.x + wCol.Size.x * 0.5f, wT.Position.y + wCol.Size.y * 0.5f};

            if (pMax.x > cMin.x && pMin.x < cMax.x && pMax.y > cMin.y && pMin.y < cMax.y) {
                float overlapY_top = cMax.y - pMin.y;
                float overlapY_bottom = pMax.y - cMin.y;
                float overlapX_left = pMax.x - cMin.x;
                float overlapX_right = cMax.x - pMin.x;

                float minOverlap = std::min({overlapY_top, overlapY_bottom, overlapX_left, overlapX_right});

                if (minOverlap == overlapY_top) carT.Position.y = cMax.y + carCol.Size.y * 0.5f;
                else if (minOverlap == overlapY_bottom) carT.Position.y = cMin.y - carCol.Size.y * 0.5f;
                else if (minOverlap == overlapX_left) carT.Position.x = cMin.x - carCol.Size.x * 0.5f;
                else carT.Position.x = cMax.x + carCol.Size.x * 0.5f;

                carC.Speed *= -wall.Bounciness;
            }
        });

    GetRegistry().View<TransformComponent, ColliderComponent, CheckpointComponent>(
        [&](auto entity, TransformComponent &cT, ColliderComponent &cCol, CheckpointComponent &cp) {
            glm::vec2 cMin = {cT.Position.x - cCol.Size.x * 0.5f, cT.Position.y - cCol.Size.y * 0.5f};
            glm::vec2 cMax = {cT.Position.x + cCol.Size.x * 0.5f, cT.Position.y + cCol.Size.y * 0.5f};

            if (pMax.x > cMin.x && pMin.x < cMax.x && pMax.y > cMin.y && pMin.y < cMax.y) {
                if (cp.IsHalfwayPoint) {
                    carC.PassedHalfway = true;
                } else if (cp.IsFinishLine && carC.PassedHalfway) {
                    carC.CurrentLap++;
                    if (carC.CurrentLapTime < carC.BestLapTime && carC.CurrentLapTime > 1.0f) {
                        carC.BestLapTime = carC.CurrentLapTime;
                    }
                    carC.CurrentLapTime = 0.0f;
                    carC.PassedHalfway = false;
                }
            }
        });
}

void RacingScene::UpdateCamera(float dt) {
    if (!GetRegistry().Has<TransformComponent>(m_Car)) return;
    auto &carT = GetRegistry().Get<TransformComponent>(m_Car);
    auto &carC = GetRegistry().Get<CarComponent>(m_Car);

    glm::vec3 camPos = GetCamera().GetPosition();

    float radRotation = glm::radians(carT.Rotation);
    glm::vec2 forward = {std::cos(radRotation), std::sin(radRotation)};

    glm::vec3 target = {
        carT.Position.x + forward.x * (carC.Speed * 0.2f),
        carT.Position.y + forward.y * (carC.Speed * 0.2f),
        0.0f
    };

    glm::vec3 newPos = camPos + (target - camPos) * 4.0f * dt;
    GetCamera().SetPosition(newPos);
}


