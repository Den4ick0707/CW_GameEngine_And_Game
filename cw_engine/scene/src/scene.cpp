#include "scene.h"
#include "components.h"
#include "Renderer2D.h"
#include "render_command.h"

#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>

namespace Engine::Scene {

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

    void Scene::OnUpdate(float dt) {
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

}