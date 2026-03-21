#pragma once

#include "scene.h"
#include <unordered_map>
#include <memory>
#include <string>

namespace Engine::Scene {

    /// @brief Менеджер сцен — перемикання між рівнями / станами гри.
    ///
    /// @code
    ///   SceneManager::Add("Menu",   std::make_shared<MenuScene>());
    ///   SceneManager::Add("Level1", std::make_shared<Level1>());
    ///   SceneManager::LoadScene("Menu");
    ///
    ///   // У game loop:
    ///   SceneManager::GetActive()->OnUpdate(dt);
    ///   SceneManager::GetActive()->OnRender();
    /// @endcode
    class SceneManager {
    public:
        /// @brief Зареєструвати сцену під іменем.
        static void Add(const std::string& name, std::shared_ptr<Scene> scene);

        /// @brief Завантажити (активувати) сцену за іменем.
        static void LoadScene(const std::string& name);

        /// @brief Отримати поточну активну сцену.
        static Scene* GetActive();

        /// @brief Ім'я поточної сцени.
        static const std::string& GetActiveName();

        /// @brief Видалити всі зареєстровані сцени.
        static void Clear();

    private:
        static std::unordered_map<std::string, std::shared_ptr<Scene>> s_Scenes;
        static std::string s_ActiveName;
    };

} // namespace Engine::Scene