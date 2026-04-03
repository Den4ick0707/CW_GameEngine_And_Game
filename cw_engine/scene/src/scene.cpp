#include "scene.h"
#include "components.h"
#include "Renderer2D.h"
#include "render_command.h"
#include "component_pool.h"
#include "json.hpp"
#include <iostream>
#include <sstream>
#include <fstream>



#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>

namespace Engine::Scene {
    using json = nlohmann::json;

    Scene::Scene(std::string name)
        : m_Name(std::move(name))
        , m_Camera(-1.777f, 1.777f, -1.0f, 1.0f)
    {}

    Entity Scene::CreateEntity(const std::string& name) {
        // FIX: Create() без аргументів, потім вручну додаємо Tag
        Entity e = m_Registry.Create();
        m_Registry.Add<TagComponent>(e, name);
        m_Registry.Add<TransformComponent>(e);
        m_Registry.Add<ActiveComponent>(e);
        return e;
    }

    void Scene::DestroyEntity(Entity entity) {
        m_Registry.Destroy(entity);
    }

    void Scene::Update(float dt) {
        UpdatePhysicsSystem(dt);
    }

    void Scene::OnRender() {
        RenderSystem();
    }

    void Scene::UpdatePhysicsSystem(float dt) {
        m_Registry.View<TransformComponent,
                        RigidbodyComponent,
                        ActiveComponent>(
            [dt, this](EntityID,
                       TransformComponent& t,
                       RigidbodyComponent& rb,
                       ActiveComponent& active)
            {
                if (!active.Active || rb.IsStatic) return;

                if (rb.UseGravity)
                    rb.Acceleration.y += m_Gravity;

                rb.Velocity   += rb.Acceleration * dt;
                rb.Velocity   *= (1.0f - rb.Drag * dt);
                t.Position.x  += rb.Velocity.x * dt;
                t.Position.y  += rb.Velocity.y * dt;

                rb.Acceleration = { 0.0f, 0.0f };
            });
    }

    void Scene::RenderSystem() {
        using namespace Engine::Graphics;

        Renderer2D::BeginScene(m_Camera);

        struct DrawCall {
            glm::vec3 Pos;
            glm::vec2 Scale;
            float     Rotation;
            glm::vec4 Color;
            std::shared_ptr<Engine::Graphics::Texture> Tex;
            int ZLayer;
        };

        std::vector<DrawCall> drawCalls;

        m_Registry.View<TransformComponent,
                        SpriteRendererComponent,
                        ActiveComponent>(
            [&](EntityID,
                TransformComponent& t,
                SpriteRendererComponent& s,
                ActiveComponent& active)
            {
                if (!active.Active || !s.Visible) return;

                drawCalls.push_back({
                    t.Position,
                    t.Scale,
                    t.Rotation,
                    s.Color,
                    s.Texture,
                    s.ZLayer
                });
            });

        std::sort(drawCalls.begin(), drawCalls.end(),
            [](const DrawCall& a, const DrawCall& b) {
                return a.ZLayer < b.ZLayer;
            });

        for (const auto& dc : drawCalls) {
            if (dc.Tex)
                Renderer2D::DrawRotatedQuad(dc.Pos, dc.Scale, dc.Rotation, dc.Tex, dc.Color);
            else
                Renderer2D::DrawRotatedQuad(dc.Pos, dc.Scale, dc.Rotation, dc.Color);
        }

        Renderer2D::EndScene();
    }
    void Scene::UpdateTransforms() {
        // 1. Оновлюємо локальні матриці всіх об'єктів
        for (auto entityID : m_Registry.GetAllEntities()) {
            if (m_Registry.Has<TransformComponent>(entityID)) {
                auto& tc = m_Registry.Get<TransformComponent>(entityID);
                tc.CalculateLocalTransform();
                tc.GlobalTransform = tc.LocalTransform; // За замовчуванням (без батька)
            }
        }

        // 2. Оновлюємо дітей відносно кореневих батьків
        for (auto entityID : m_Registry.GetAllEntities()) {
            if (m_Registry.Has<RelationshipComponent>(entityID)) {
                auto& rel = m_Registry.Get<RelationshipComponent>(entityID);

                // Шукаємо "кореневі" об'єкти (у яких немає батька)
                if (rel.Parent == 0 && m_Registry.Has<TransformComponent>(entityID)) {
                    auto& tc = m_Registry.Get<TransformComponent>(entityID);
                    UpdateChildTransforms(entityID, tc.GlobalTransform);
                }
            }
        }
    }

    void Scene::UpdateChildTransforms(uint32_t entityID, const glm::mat4& parentGlobalTransform) {
        auto& rel = m_Registry.Get<RelationshipComponent>(entityID);
        uint32_t currentChildID = rel.FirstChild;

        while (currentChildID != 0) {
            if (m_Registry.Has<TransformComponent>(currentChildID)) {
                auto& childTc = m_Registry.Get<TransformComponent>(currentChildID);

                // Дитина слідує за батьком
                childTc.GlobalTransform = parentGlobalTransform * childTc.LocalTransform;

                // Йдемо глибше по ієрархії
                UpdateChildTransforms(currentChildID, childTc.GlobalTransform);
            }
            auto& childRel = m_Registry.Get<RelationshipComponent>(currentChildID);
            currentChildID = childRel.NextSibling;
        }
    }
    void Scene::Serialize(const std::string& filepath) {
        json sceneData;
        json entitiesArray = json::array();

        // Проходимо по всіх сутностях у реєстрі
        for (auto entityID : m_Registry.GetAllEntities()) {
            json entityData;
            entityData["EntityID"] = entityID;

            // Зберігаємо Tag (Ім'я)
            if (m_Registry.Has<TagComponent>(entityID)) {
                entityData["TagComponent"]["Tag"] = m_Registry.Get<TagComponent>(entityID).Name;
            }

            // Зберігаємо ваш 2D Transform
            if (m_Registry.Has<TransformComponent>(entityID)) {
                auto& tc = m_Registry.Get<TransformComponent>(entityID);
                entityData["TransformComponent"] = {
                    {"Position", {tc.Position.x, tc.Position.y, tc.Position.z}},
                    {"Rotation", tc.Rotation},
                    {"Scale", {tc.Scale.x, tc.Scale.y}}
                };
            }

            // Зберігаємо Ієрархію
            if (m_Registry.Has<RelationshipComponent>(entityID)) {
                auto& rel = m_Registry.Get<RelationshipComponent>(entityID);
                entityData["RelationshipComponent"] = {
                    {"Parent", rel.Parent},
                    {"FirstChild", rel.FirstChild},
                    {"NextSibling", rel.NextSibling}
                };
            }

            entitiesArray.push_back(entityData);
        }

        sceneData["Entities"] = entitiesArray;

        // Запис у файл
        std::ofstream file(filepath);
        if (file.is_open()) {
            file << sceneData.dump(4); // 4 пробіли для гарного форматування
            std::cout << "Scene saved to " << filepath << std::endl;
        } else {
            std::cerr << "Failed to open file for saving: " << filepath << std::endl;
        }
    }

    // --- ЗАВАНТАЖЕННЯ (ДЕСЕРІАЛІЗАЦІЯ) ---
    void Scene::Deserialize(const std::string& filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            std::cerr << "Failed to open file for loading: " << filepath << std::endl;
            return;
        }

        json sceneData;
        file >> sceneData;

        // 1. ВАЖЛИВО: Очищення старої сцени!
        // Оскільки CreateEntity видає нові ID по порядку,
        // реєстр має бути пустим, щоб завантажені ID збіглися зі збереженими.
        // Якщо у вас є метод m_Registry.Clear(); - викличте його тут.

        if (sceneData.contains("Entities")) {
            for (const auto& entityData : sceneData["Entities"]) {

                // Спочатку шукаємо ім'я в JSON, щоб передати його у ваш метод
                std::string entityName = "Entity";
                if (entityData.contains("TagComponent")) {
                    entityName = entityData["TagComponent"]["Tag"];
                }

                // 2. ВИКОРИСТОВУЄМО ВАШ МЕТОД (Він сам створює Entity та додає Tag і Transform)
                Entity entity = CreateEntity(entityName);

                // 3. Перезаписуємо дані TransformComponent
                if (entityData.contains("TransformComponent")) {
                    // ЗВЕРНІТЬ УВАГУ: використовуємо Get (або аналог з вашого реєстру), бо компонент вже є
                    auto& tc = m_Registry.Get<TransformComponent>(entity);
                    auto& tcData = entityData["TransformComponent"];

                    tc.Position = { tcData["Position"][0], tcData["Position"][1], tcData["Position"][2] };
                    tc.Rotation = tcData["Rotation"];
                    tc.Scale = { tcData["Scale"][0], tcData["Scale"][1] };
                }

                // 4. Додаємо ієрархію (ваш CreateEntity її НЕ додає автоматично, тому робимо Add)
                if (entityData.contains("RelationshipComponent")) {
                    RelationshipComponent rel;
                    auto& relData = entityData["RelationshipComponent"];

                    rel.Parent = relData["Parent"];
                    rel.FirstChild = relData["FirstChild"];
                    rel.NextSibling = relData["NextSibling"];

                    m_Registry.Add<RelationshipComponent>(entity, rel);
                }
            }
            std::cout << "Scene loaded successfully from " << filepath << std::endl;
        }
    }


}