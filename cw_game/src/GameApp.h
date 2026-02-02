#pragma once

#include <EngineCore/Application.h>
#include <Graphics/OrthographicCamera.h>
#include <Scene/Scene.h>
#include <memory>

/**
 * @class GameApp
 * @brief Основний клас вашої гри. Успадковує Engine::Application.
 *
 * Цей клас є точкою входу вашої гри. Він відповідає за створення сцени,
 * наповнення її ігровими об'єктами (гравець, платформи) та реалізує
 * основні методи ігрового циклу OnUpdate() та OnRender().
 */
class GameApp : public Engine::Application {
public:
    /**
     * @brief Конструктор гри. Тут створюється та налаштовується ігровий рівень.
     */
    GameApp();
    /**
     * @brief Деструктор гри.
     */
    ~GameApp() override;

    /**
     * @brief Оновлює стан гри на кожному кадрі.
     * @param dt Час кадру.
     */
    void OnUpdate(float dt) override;

    /**
     * @brief Рендерить ігрову сцену на кожному кадрі.
     */
    void OnRender() override;

private:
    /**
     * @brief Допоміжна функція для створення платформ на рівні.
     * @param name Ім'я платформи.
     * @param position Позиція платформи.
     * @param scale Масштаб платформи.
     * @param color Колір платформи.
     */
    void CreatePlatform(const std::string& name, const glm::vec3& position, const glm::vec3& scale, const glm::vec4& color);

    /// @brief Ігрова камера, що визначає, яку частину світу бачить гравець.
    Engine::Graphics::OrthographicCamera m_Camera;
    /// @brief Розумний вказівник на об'єкт сцени, що керує всіма ігровими об'єктами.
    std::unique_ptr<Engine::Scene::Scene> m_Scene;
};
