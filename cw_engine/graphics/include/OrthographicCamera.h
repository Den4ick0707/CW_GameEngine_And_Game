#pragma once

#include <glm/glm.hpp>

namespace Engine::Graphics {

    /// @brief 2D ортографічна камера.
    class OrthographicCamera {
    public:
        OrthographicCamera(float left, float right, float bottom, float top);

        /// @brief Оновити межі проекції (при зміні розміру вікна).
        void SetProjection(float left, float right, float bottom, float top);

        // ── Позиція та поворот ────────────────────────────────────────────

        [[nodiscard]] const glm::vec3& GetPosition() const { return m_Position; }
        [[nodiscard]] float            GetRotation() const { return m_Rotation; }

        void SetPosition(const glm::vec3& pos) {
            m_Position = pos;
            Recalculate();
        }

        void SetRotation(float degrees) {
            m_Rotation = degrees;
            Recalculate();
        }

        /// @brief Встановити позицію і поворот одночасно (один Recalculate).
        void SetTransform(const glm::vec3& pos, float degrees) {
            m_Position = pos;
            m_Rotation = degrees;
            Recalculate();
        }

        // ── Матриці ───────────────────────────────────────────────────────

        [[nodiscard]] const glm::mat4& GetProjectionMatrix()     const { return m_Projection; }
        [[nodiscard]] const glm::mat4& GetViewMatrix()           const { return m_View; }
        [[nodiscard]] const glm::mat4& GetViewProjectionMatrix() const { return m_ViewProjection; }

    private:
        void Recalculate();

        glm::mat4 m_Projection;
        glm::mat4 m_View           = glm::mat4(1.0f);
        glm::mat4 m_ViewProjection = glm::mat4(1.0f);

        glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };
        float     m_Rotation = 0.0f;
    };

} // namespace Engine::Graphics