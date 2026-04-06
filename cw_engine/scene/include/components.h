#pragma once

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <string>
#include <memory>

// Forward declaration — не тягнемо весь graphics у scene
namespace Engine::Graphics { class Texture; }

namespace Engine::Scene {

    // ─────────────────────────────────────────────────────────────────────────
    // Базові компоненти рушія
    // ─────────────────────────────────────────────────────────────────────────

    /// @brief Людське ім'я сутності (для дебагу та редактора).
    struct TagComponent {
        std::string Name = "Entity";

        TagComponent() = default;
        explicit TagComponent(std::string name) : Name(std::move(name)) {}
    };

    struct TransformComponent {
        glm::vec3 Position = { 0.0f, 0.0f, 0.0f };
        float     Rotation = 0.0f;              ///< Градуси, вісь Z
        glm::vec2 Scale    = { 1.0f, 1.0f };

        // Додаємо кешовані матриці для ієрархії та рендерингу
        glm::mat4 LocalTransform = glm::mat4(1.0f);
        glm::mat4 GlobalTransform = glm::mat4(1.0f);

        TransformComponent() = default;
        TransformComponent(const glm::vec3& pos,
                           float rot = 0.0f,
                           const glm::vec2& scale = { 1.0f, 1.0f })
            : Position(pos), Rotation(rot), Scale(scale) {}

        /// @brief Зручний доступ до 2D позиції.
        [[nodiscard]] glm::vec2 GetPosition2D() const {
            return { Position.x, Position.y };
        }

        /// @brief Обчислює матрицю трансформації відносно батька.
        void CalculateLocalTransform() {
            // 1. Переміщення (Translate)
            LocalTransform = glm::translate(glm::mat4(1.0f), Position);

            // 2. Обертання (Rotate) - GLM очікує радіани, тому конвертуємо градуси
            LocalTransform = glm::rotate(LocalTransform, glm::radians(Rotation), glm::vec3(0.0f, 0.0f, 1.0f));

            // 3. Масштабування (Scale) - додаємо 1.0f для осі Z
            LocalTransform = glm::scale(LocalTransform, glm::vec3(Scale.x, Scale.y, 1.0f));
        }
    };

    struct RelationshipComponent {
        uint32_t Parent = 0;
        uint32_t FirstChild = 0;
        uint32_t NextSibling = 0;
    };

}



    /// @brief Візуальне представлення — колір або текстура.
    struct SpriteRendererComponent {
        glm::vec4 Color   = { 1.0f, 1.0f, 1.0f, 1.0f };
        std::shared_ptr<Engine::Graphics::Texture> Texture = nullptr;

        /// @brief Шар для сортування (більше = ближче до камери).
        int ZLayer  = 0;

        /// @brief Видимість об'єкта.
        bool Visible = true;

        SpriteRendererComponent() = default;
        explicit SpriteRendererComponent(const glm::vec4& color) : Color(color) {}
    };

    /// @brief Фізичне тіло для базової 2D фізики.
    struct RigidbodyComponent {
        glm::vec2 Velocity     = { 0.0f, 0.0f };
        glm::vec2 Acceleration = { 0.0f, 0.0f };

        float Mass    = 1.0f;
        float Drag    = 0.0f;   ///< Коефіцієнт тертя повітря [0, 1]

        bool IsStatic    = false;  ///< Не рухається
        bool UseGravity  = false;  ///< Застосовувати гравітацію

        /// @brief Застосувати імпульс (миттєву зміну швидкості).
        void ApplyImpulse(const glm::vec2& impulse) {
            if (!IsStatic) Velocity += impulse / Mass;
        }

        /// @brief Застосувати силу (накопичується в Acceleration).
        void ApplyForce(const glm::vec2& force) {
            if (!IsStatic) Acceleration += force / Mass;
        }
    };

    /// @brief AABB колайдер для виявлення зіткнень.
    struct ColliderComponent {
        glm::vec2 Size    = { 1.0f, 1.0f };  ///< Розміри (ширина, висота)
        glm::vec2 Offset  = { 0.0f, 0.0f };  ///< Зміщення відносно Transform

        bool IsTrigger = false;  ///< Trigger не блокує, тільки сповіщає

        /// @brief Обчислити мінімальну точку AABB у світі.
        [[nodiscard]] glm::vec2 GetMin(const glm::vec2& worldPos) const {
            return worldPos + Offset - Size * 0.5f;
        }

        /// @brief Обчислити максимальну точку AABB у світі.
        [[nodiscard]] glm::vec2 GetMax(const glm::vec2& worldPos) const {
            return worldPos + Offset + Size * 0.5f;
        }

        /// @brief Перевірити перетин двох AABB.
        [[nodiscard]] bool Intersects(const glm::vec2& myPos,
                                      const ColliderComponent& other,
                                      const glm::vec2& otherPos) const {
            glm::vec2 minA = GetMin(myPos),    maxA = GetMax(myPos);
            glm::vec2 minB = other.GetMin(otherPos), maxB = other.GetMax(otherPos);

            return minA.x < maxB.x && maxA.x > minB.x &&
                   minA.y < maxB.y && maxA.y > minB.y;
        }
    };

    /// @brief Маркер — позначити entity як "активне" або "вимкнене".
    struct ActiveComponent {
        bool Active = true;
    };

// namespace Engine::Scene