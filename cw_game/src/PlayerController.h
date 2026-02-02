#pragma once

#include <Scene/Component.h>

// Попередні оголошення для уникнення включення повних заголовків,
// що прискорює компіляцію.
namespace Engine {
    namespace Physics {
        class RigidBody2D;
    }
    class BoxCollider2D;
}

/**
 * @class PlayerController
 * @brief Компонент, що реалізує логіку керування гравцем.
 *
 * Цей клас зчитує ввід з клавіатури та перетворює його на дії,
 * такі як рух вліво/вправо та стрибок. Він не керує фізикою напряму,
 * а лише надає "команди" компоненту RigidBody2D.
 */
class PlayerController : public Engine::Scene::Component {
public:
    float MoveSpeed = 10.0f;     ///< Швидкість руху гравця по горизонталі.
    float JumpForce = 10.0f;      ///< Сила стрибка.
    float RotationSpeed = 180.0f;///< Швидкість обертання (для розваги).

    /**
     * @brief Ініціалізація контролера. Викликається один раз при створенні.
     */
    void OnStart() override;

    /**
     * @brief Оновлення логіки контролера. Викликається на кожному кадрі.
     * @param dt Час кадру.
     */
    void OnUpdate(float dt) override;

private:
    /**
     * @brief Перевіряє, чи стоїть гравець на землі.
     *
     * Це потрібно, щоб дозволити стрибок тільки з поверхні,
     * а не нескінченно в повітрі.
     */
    void CheckIfGrounded();

    /// @brief Кешований вказівник на компонент RigidBody2D для ефективності.
    Engine::Physics::RigidBody2D* m_RigidBody = nullptr;
    /// @brief Прапорець, що показує, чи стоїть гравець на землі.
    bool m_IsGrounded = false;
};
