#include "PlayerController.h"
#include <EngineCore/Input.h>
#include <Physics/RigidBody2D.h>
#include <Physics/BoxCollider2D.h>
#include <Scene/GameObject.h>
#include <Scene/Scene.h>
#include <GLFW/glfw3.h>

// Робимо імена з просторів імен рушія доступними в цьому файлі
using namespace Engine;
using namespace Engine::Physics;

void PlayerController::OnStart() {
    m_RigidBody = m_Owner->GetComponent<RigidBody2D>();
}

void PlayerController::OnUpdate(float dt) {
    if (!m_RigidBody) return;

    CheckIfGrounded();

    // 1. Рух вліво-вправо
    float moveInput = 0.0f;
    if (Input::IsKeyPressed(GLFW_KEY_A)) moveInput -= 1.0f;
    if (Input::IsKeyPressed(GLFW_KEY_D)) moveInput += 1.0f;
    m_RigidBody->Velocity.x = moveInput * MoveSpeed;

    // 2. Стрибок (тільки якщо на землі)
    if (Input::IsKeyPressed(GLFW_KEY_W) && m_IsGrounded) {
        m_RigidBody->Velocity.y = JumpForce;
    }

    // 3. Поворот
    if (Input::IsKeyPressed(GLFW_KEY_Q)) m_Owner->Rotation.z += RotationSpeed * dt;
    if (Input::IsKeyPressed(GLFW_KEY_E)) m_Owner->Rotation.z -= RotationSpeed * dt;
}

void PlayerController::CheckIfGrounded() {
    m_IsGrounded = false;
    auto myCollider = m_Owner->GetComponent<BoxCollider2D>();
    if (!myCollider) return;

    glm::vec3 checkPos = m_Owner->Position;
    checkPos.y -= 0.1f;

    AABB playerCheckAABB = myCollider->GetAABB(checkPos, m_Owner->Scale);

    for (const auto& otherObjPtr : m_Owner->GetScene()->GetAllGameObjects()) {
        auto otherObj = otherObjPtr.get();
        if (otherObj == m_Owner) continue;

        auto otherCollider = otherObj->GetComponent<BoxCollider2D>();
        if (!otherCollider || otherCollider->IsTrigger) continue;

        AABB otherAABB = otherCollider->GetAABB(otherObj->Position, otherObj->Scale);

        if (playerCheckAABB.Intersects(otherAABB)) {
            m_IsGrounded = true;
            return;
        }
    }
}
