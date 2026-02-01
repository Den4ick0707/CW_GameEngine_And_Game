#include "pch.h"
#include "OrthographicCamera.h"
#include <glm/gtc/matrix_transform.hpp>

namespace Graphics {

    OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top)
        : m_ProjectionMatrix(glm::ortho(left, right, bottom, top, -1.0f, 1.0f)), m_ViewMatrix(1.0f)
    {
        m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
    }

    void OrthographicCamera::RecalculateViewMatrix() {
        // Створюємо матрицю трансформації камери (Позиція + Поворот)
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Position) *
                              glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation), glm::vec3(0, 0, 1));

        // View Matrix - це інверсія трансформації (світ рухається протилежно камері)
        m_ViewMatrix = glm::inverse(transform);
        
        // Оновлюємо кеш
        m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
    }
}