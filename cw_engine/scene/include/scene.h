#pragma once

#include "registry.h"
#include "components.h"
#include "../../../cw_engine/graphics/include/OrthographicCamera.h"
#include "../../../cw_engine/graphics/include/Renderer2D.h"

#include <string>
#include <functional>

namespace Engine::Scene {

    /// @brief Контейнер для одного ігрового рівня / стану.
    ///
    /// @details Scene = Registry + Camera + Update/Render системи.
    /// Наслідуй або використовуй напряму:
    ///
    /// @code
    ///   Scene scene("Level 1");
    ///   Entity player = scene.CreateEntity("Player");
    ///   scene.GetRegistry().Add<TransformComponent>(player);
    ///
    ///   scene.OnUpdate(dt);   // у Application::Update
    ///   scene.OnRender();     // у Application::Render
    /// @endcode
    class Scene {
    public:
        explicit Scene(std::string name = "Untitled");
        virtual ~Scene() = default;

        Scene(const Scene&)            = delete;
        Scene& operator=(const Scene&) = delete;

        // ── Entity shortcuts ──────────────────────────────────────────────

        /// @brief Створити сутність з ім'ям і TransformComponent.
        Entity CreateEntity(const std::string& name = "Entity");

        /// @brief Знищити сутність.
        void DestroyEntity(Entity entity);

        // ── Lifecycle ─────────────────────────────────────────────────────

        virtual void OnUpdate(float dt);
        virtual void OnRender();

        // ── Доступ ────────────────────────────────────────────────────────

        [[nodiscard]] Registry&       GetRegistry()       { return m_Registry; }
        [[nodiscard]] const Registry& GetRegistry() const { return m_Registry; }

        [[nodiscard]] Engine::Graphics::OrthographicCamera& GetCamera()
            { return m_Camera; }

        [[nodiscard]] const std::string& GetName() const { return m_Name; }

        void SetGravity(float g) { m_Gravity = g; }

    private:
        // ── Вбудовані системи ─────────────────────────────────────────────

        void UpdatePhysicsSystem(float dt);
        void RenderSystem();

        std::string m_Name;
        Registry    m_Registry;

        Engine::Graphics::OrthographicCamera m_Camera;

        float m_Gravity = -9.8f;
    };

} // namespace Engine::Scene