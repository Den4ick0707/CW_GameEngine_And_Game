#include "RigidBody2D.h"
#include "Scene/GameObject.h"
#include "Scene/Scene.h"
#include "BoxCollider2D.h"

namespace Engine {
    namespace Physics {

        using namespace Scene;

        const glm::vec2 GRAVITY = { 0.0f, -9.81f };

        void RigidBody2D::OnUpdate(float dt) {
            if (UseGravity) {
                Velocity += GRAVITY * GravityScale * dt;
            }

            auto myCollider = m_Owner->GetComponent<BoxCollider2D>();
            if (myCollider) {
                glm::vec3 nextPos = m_Owner->Position + glm::vec3(Velocity.x, Velocity.y, 0.0f) * dt;

                for (const auto& otherObjPtr : m_Owner->GetScene()->GetAllGameObjects()) {
                    auto otherObj = otherObjPtr.get();
                    if (otherObj == m_Owner) continue;

                    auto otherCollider = otherObj->GetComponent<BoxCollider2D>();
                    if (!otherCollider || otherCollider->IsTrigger) continue;

                    AABB myNextAABB_X = myCollider->GetAABB({nextPos.x, m_Owner->Position.y, m_Owner->Position.z}, m_Owner->Scale);
                    AABB otherAABB = otherCollider->GetAABB(otherObj->Position, otherObj->Scale);

                    if (myNextAABB_X.Intersects(otherAABB)) {
                        Velocity.x = 0;
                        nextPos.x = m_Owner->Position.x;
                    }

                    AABB myNextAABB_Y = myCollider->GetAABB({m_Owner->Position.x, nextPos.y, m_Owner->Position.z}, m_Owner->Scale);
                    if (myNextAABB_Y.Intersects(otherAABB)) {
                        if (Velocity.y < 0) {
                            m_Owner->Position.y = otherAABB.Max.y + (myCollider->Size.y * m_Owner->Scale.y) / 2.0f - myCollider->Offset.y;
                        } else if (Velocity.y > 0) {
                            m_Owner->Position.y = otherAABB.Min.y - (myCollider->Size.y * m_Owner->Scale.y) / 2.0f - myCollider->Offset.y;
                        }
                        Velocity.y = 0;
                        nextPos.y = m_Owner->Position.y;
                    }
                }
                m_Owner->Position = nextPos;
            } else {
                m_Owner->Position.x += Velocity.x * dt;
                m_Owner->Position.y += Velocity.y * dt;
            }
        }

        void RigidBody2D::AddForce(const glm::vec2& force) {
            Velocity += force;
        }
    }
}
