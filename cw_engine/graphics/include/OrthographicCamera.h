#pragma once

#include <glm/glm.hpp>

namespace Engine::Graphics {

    /// @brief A 2D Orthographic Camera.
    /// @details
    /// This camera uses orthographic projection, meaning there is no perspective
    /// (objects don't get smaller as they move away on the Z axis).
    /// Perfect for 2D games, UI rendering, and isometric views.
    class OrthographicCamera {
    public:
        /// @brief Creates an orthographic camera with specific boundaries.
        OrthographicCamera(float left, float right, float bottom, float top);

        /// @brief Updates the projection boundaries (useful when the window is resized).
        void SetProjection(float left, float right, float bottom, float top);

        [[nodiscard]] const glm::vec3& GetPosition() const { return m_Position; }

        /// @brief Sets the camera position and rotation at once to avoid double recalculation.
        void SetTransform(const glm::vec3& position, float rotation) {
            m_Position = position;
            m_Rotation = rotation;
            RecalculateViewMatrix();
        }

        void SetPosition(const glm::vec3& position) {
            m_Position = position;
            RecalculateViewMatrix();
        }

        [[nodiscard]] float GetRotation() const { return m_Rotation; }

        void SetRotation(float rotation) {
            m_Rotation = rotation;
            RecalculateViewMatrix();
        }

        [[nodiscard]] const glm::mat4& GetProjectionMatrix()     const { return m_ProjectionMatrix; }
        [[nodiscard]] const glm::mat4& GetViewMatrix()           const { return m_ViewMatrix; }
        [[nodiscard]] const glm::mat4& GetViewProjectionMatrix() const { return m_ViewProjectionMatrix; }

    private:
        /// @brief Recalculates the View and ViewProjection matrices.
        void RecalculateViewMatrix();

    private:
        glm::mat4 m_ProjectionMatrix;
        glm::mat4 m_ViewMatrix;
        glm::mat4 m_ViewProjectionMatrix;

        glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };
        float     m_Rotation = 0.0f; ///< Camera rotation on the Z axis (in degrees)
    };
}