#ifndef ENGINE_SCENE_H
#define ENGINE_SCENE_H

#pragma once
#include <vector>
#include <memory>
#include <string>
#include "GameObject.h"

namespace Engine {
    namespace Scene {
        /**
         * @class Scene
         * @brief Контейнер, що керує всіма ігровими об'єктами на рівні.
         *
         * Сцена є "світом" вашої гри. Вона володіє всіма GameObject,
         * відповідає за їх створення, пошук, оновлення та рендеринг.
         * Використання сцени дозволяє уникнути глобальних змінних та
         * забезпечує централізоване керування життєвим циклом об'єктів.
         */
        class Scene {
        public:
            /**
             * @brief Конструктор сцени.
             */
            Scene();
            /**
             * @brief Деструктор сцени. Автоматично видаляє всі GameObject,
             *        що були створені в ній, завдяки використанню std::unique_ptr.
             */
            ~Scene();

            /**
             * @brief Оновлює логіку всіх ігрових об'єктів на сцені.
             * @param deltaTime Час, що минув з попереднього кадру.
             */
            void OnUpdate(float deltaTime);

            /**
             * @brief Рендерить (малює) всі ігрові об'єкти на сцені.
             */
            void OnRender();

            /**
             * @brief Створює новий ігровий об'єкт (GameObject) на сцені.
             * @param name Ім'я об'єкта для ідентифікації.
             * @return Вказівник на створений об'єкт.
             */
            GameObject* CreateGameObject(const std::string& name = "GameObject");

            /**
             * @brief Знаходить ігровий об'єкт на сцені за його іменем.
             * @param name Ім'я об'єкта для пошуку.
             * @return Вказівник на знайдений об'єкт або nullptr, якщо об'єкт не знайдено.
             */
            GameObject* FindGameObjectByName(const std::string& name);

            /**
             * @brief Повертає список всіх ігрових об'єктів на сцені.
             * @return Константне посилання на вектор з розумними вказівниками на GameObject.
             */
            const std::vector<std::unique_ptr<GameObject>>& GetAllGameObjects() const;

        private:
            /// @brief Вектор, що зберігає всі ігрові об'єкти.
            /// Використання std::unique_ptr гарантує автоматичне звільнення пам'яті.
            std::vector<std::unique_ptr<GameObject>> m_GameObjects;
        };
    }
}

#endif // ENGINE_SCENE_H
