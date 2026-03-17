//
// Created by onigirya on 17.03.26.
//

#ifndef COURSE_WORK_DARYEV_COMPONENTS_H
#define COURSE_WORK_DARYEV_COMPONENTS_H
#include <glm/glm.hpp>
#include <string>
#include <memory>

// Forward declaration щоб не тягнути весь graphics у scene
namespace Engine::Graphics { class Texture; }

namespace Engine::Scene {

    struct TagComponent {
        std::string Name = "Entity";
    };

    struct TransformComponent {
        glm::vec3 Position = { 0.0f, 0.0f, 0.0f };
        float     Rotation = 0.0f;   // degrees, Z-axis
        glm::vec2 Scale    = { 1.0f, 1.0f };
    };

    struct SpriteRendererComponent {
        glm::vec4 Color  = { 1.0f, 1.0f, 1.0f, 1.0f };
        std::shared_ptr<Engine::Graphics::Texture> Texture = nullptr;
        int ZLayer = 0; // для сортування
    };

    struct RigidbodyComponent {
        glm::vec2 Velocity     = { 0.0f, 0.0f };
        glm::vec2 Acceleration = { 0.0f, 0.0f };
        float     Mass         = 1.0f;
        bool      IsStatic     = false;
    };

    struct ColliderComponent {
        glm::vec2 Size   = { 1.0f, 1.0f }; // AABB half-extents
        glm::vec2 Offset = { 0.0f, 0.0f };
        bool      IsTrigger = false;
    };
}
#endif //COURSE_WORK_DARYEV_COMPONENTS_H