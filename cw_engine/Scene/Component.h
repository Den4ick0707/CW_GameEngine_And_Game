#ifndef COURSEWORK_GAMEENGINE_COMPONENT_H
#define COURSEWORK_GAMEENGINE_COMPONENT_H

namespace Engine {
    namespace Scene {
        class GameObject; // Попереднє оголошення, щоб уникнути циклічної залежності

        /**
         * @class Component
         * @brief Базовий клас для всіх компонентів, що розширюють функціональність GameObject.
         *
         * Компоненти - це будівельні блоки, з яких складається поведінка ігрових об'єктів.
         * Кожен компонент відповідає за одну конкретну річ:
         * - SpriteRenderer: малює спрайт.
         * - RigidBody2D: реалізує фізику.
         * - PlayerController: реалізує логіку керування гравцем.
         *
         * Цей клас надає віртуальні методи, які викликаються GameObject в певні моменти
         * його життєвого циклу (створення, оновлення, рендеринг).
         */
        class Component {
        public:
            /// @brief Вказівник на GameObject, якому належить цей компонент.
            /// Встановлюється автоматично при виклику GameObject::AddComponent().
            GameObject* m_Owner = nullptr;

            /**
             * @brief Віртуальний деструктор.
             */
            virtual ~Component() = default;

            /**
             * @brief Викликається один раз, одразу після створення компонента та додавання його до GameObject.
             * Ідеальне місце для ініціалізації та кешування посилань на інші компоненти.
             */
            virtual void OnStart() {}

            /**
             * @brief Викликається на кожному кадрі. Тут повинна знаходитись основна логіка компонента.
             * @param dt Час, що минув з попереднього кадру.
             */
            virtual void OnUpdate(float dt) {}

            /**
             * @brief Викликається на кожному кадрі під час фази рендерингу.
             * Використовується компонентами, що відповідають за візуалізацію (напр., SpriteRenderer).
             */
            virtual void OnRender() {}
        };
    }
}

#endif //COURSEWORK_GAMEENGINE_COMPONENT_H
