#ifndef ENGINE_RIGIDBODY2D_H
#define ENGINE_RIGIDBODY2D_H

#pragma once
#include "Scene/Component.h"
#include "glm/glm.hpp"

namespace Engine {
    namespace Physics {
        /**
         * @class RigidBody2D
         * @brief Компонент, що надає об'єкту фізичні властивості у 2D-просторі.
         *
         * Відповідає за рух об'єкта під дією швидкості та гравітації.
         * Також цей компонент містить логіку для виявлення та вирішення колізій,
         * щоб об'єкти не проходили крізь стіни та стояли на платформах.
         */
        class RigidBody2D : public Scene::Component {
        public:
            /// @brief Поточна швидкість об'єкта. Змінюється під дією гравітації та іншими силами.
            glm::vec2 Velocity = { 0.0f, 0.0f };

            /// @brief Множник сили гравітації. Дозволяє робити об'єкти "важчими" або "легшими".
            float GravityScale = 1.0f;

            /// @brief Прапорець, що вмикає або вимикає дію гравітації на цей об'єкт.
            bool UseGravity = true;

            /**
             * @brief Оновлює позицію об'єкта на основі його швидкості, гравітації та колізій.
             * @param dt Час кадру.
             */
            void OnUpdate(float dt) override;

            /**
             * @brief Додає миттєву силу (імпульс) до об'єкта, змінюючи його швидкість.
             * @param force Вектор сили, що застосовується.
             */
            void AddForce(const glm::vec2& force);
        };
    }
}

#endif //ENGINE_RIGIDBODY2D_H
