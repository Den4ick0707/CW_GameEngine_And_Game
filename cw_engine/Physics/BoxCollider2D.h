#pragma once
#include "../Scene/Component.h"
#include "../Scene/GameObject.h"
#include <glm/glm.hpp>

namespace Engine {

    struct AABB {
        glm::vec2 Min;
        glm::vec2 Max;

        bool Intersects(const AABB& other) const {
            return Min.x < other.Max.x && Max.x > other.Min.x &&
                   Min.y < other.Max.y && Max.y > other.Min.y;
        }
    };

    class BoxCollider2D : public Engine::Scene::Component {
    public:
        glm::vec2 Size = {1.0f, 1.0f};
        glm::vec2 Offset = {0.0f, 0.0f};
        bool IsTrigger = false;

        BoxCollider2D(glm::vec2 size, glm::vec2 offset = {0.0f, 0.0f})
            : Size(size), Offset(offset) {}

        AABB GetAABB(const glm::vec3& objectPosition, const glm::vec3& objectScale) const {
            glm::vec3 globalPos = objectPosition + glm::vec3(Offset, 0.0f);
            glm::vec2 scaledSize = Size * glm::vec2(objectScale);

            return {
                {globalPos.x - scaledSize.x / 2.0f, globalPos.y - scaledSize.y / 2.0f},
                {globalPos.x + scaledSize.x / 2.0f, globalPos.y + scaledSize.y / 2.0f}
            };
        }

        bool CheckCollision(BoxCollider2D* other) const {
            AABB myAABB = GetAABB(m_Owner->Position, m_Owner->Scale);
            AABB otherAABB = other->GetAABB(other->m_Owner->Position, other->m_Owner->Scale);
            return myAABB.Intersects(otherAABB);
        }
    };
}