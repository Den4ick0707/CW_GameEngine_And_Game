#include "scene_manager.h"
#include <iostream>
#include <cassert>

namespace Engine::Scene {

    std::unordered_map<std::string, std::shared_ptr<Scene>>
        SceneManager::s_Scenes;

    std::string SceneManager::s_ActiveName;

    void SceneManager::Add(const std::string& name,
                           std::shared_ptr<Scene> scene) {
        s_Scenes[name] = std::move(scene);
    }

    void SceneManager::LoadScene(const std::string& name) {
        auto it = s_Scenes.find(name);
        if (it == s_Scenes.end()) {
            std::cerr << "[SceneManager] Scene not found: " << name << "\n";
            return;
        }
        s_ActiveName = name;
        std::cout << "[SceneManager] Loaded: " << name << "\n";
    }

    Scene* SceneManager::GetActive() {
        if (s_ActiveName.empty()) return nullptr;
        auto it = s_Scenes.find(s_ActiveName);
        return (it != s_Scenes.end()) ? it->second.get() : nullptr;
    }

    const std::string& SceneManager::GetActiveName() {
        return s_ActiveName;
    }

    void SceneManager::Clear() {
        s_Scenes.clear();
        s_ActiveName.clear();
    }

} // namespace Engine::Scene