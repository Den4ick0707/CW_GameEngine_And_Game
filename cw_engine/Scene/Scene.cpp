#include "Scene.h"
#include <algorithm>

namespace Engine {
    namespace Scene {

        Scene::Scene() {}

        Scene::~Scene() {}

        void Scene::OnUpdate(float deltaTime) {
            for (auto& obj : m_GameObjects) {
                obj->OnUpdate(deltaTime);
            }
        }

        void Scene::OnRender() {
            for (auto& obj : m_GameObjects) {
                obj->OnRender();
            }
        }

        GameObject* Scene::CreateGameObject(const std::string& name) {
            m_GameObjects.emplace_back(std::make_unique<GameObject>(this, name));
            return m_GameObjects.back().get();
        }

        GameObject* Scene::FindGameObjectByName(const std::string& name) {
            auto it = std::find_if(m_GameObjects.begin(), m_GameObjects.end(),
                                   [&](const std::unique_ptr<GameObject>& obj) {
                                       return obj->Name == name;
                                   });

            if (it != m_GameObjects.end()) {
                return it->get();
            }

            return nullptr;
        }

        const std::vector<std::unique_ptr<GameObject>>& Scene::GetAllGameObjects() const {
            return m_GameObjects;
        }
    }
}
