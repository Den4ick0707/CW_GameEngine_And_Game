#ifndef COURSEWORK_GAMEENGINE_GAMEOBJECT_H
#define COURSEWORK_GAMEENGINE_GAMEOBJECT_H

#pragma once
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Component.h"

namespace Engine {
    namespace Scene {
        class Scene;

        class GameObject {
        public:
            std::string Name;
            glm::vec3 Position = {0.0f, 0.0f, 0.0f};
            glm::vec3 Rotation = {0.0f, 0.0f, 0.0f};
            glm::vec3 Scale = {1.0f, 1.0f, 1.0f};

            GameObject(Scene* scene, const std::string &name) : m_Scene(scene), Name(name) {}

            ~GameObject() {
                for (auto c: m_Components) delete c;
                m_Components.clear();
            }

            template<typename T, typename... Args>
            T* AddComponent(Args&&... args) {
                T* newComponent = new T(std::forward<Args>(args)...);
                newComponent->m_Owner = this;
                m_Components.push_back(newComponent);
                newComponent->OnStart();
                return newComponent;
            }

            template<typename T>
            T* GetComponent() {
                for (auto component : m_Components) {
                    if (dynamic_cast<T*>(component)) {
                        return static_cast<T*>(component);
                    }
                }
                return nullptr;
            }

            void OnUpdate(float dt) {
                for (auto component : m_Components) {
                    component->OnUpdate(dt);
                }
            }

            void OnRender() {
                for (auto component : m_Components) {
                    component->OnRender();
                }
            }

            glm::mat4 GetTransform() const {
                glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(Rotation.z), {0.0f, 0.0f, 1.0f});
                return glm::translate(glm::mat4(1.0f), Position) * rotation * glm::scale(glm::mat4(1.0f), Scale);
            }

            Scene* GetScene() const { return m_Scene; }

        private:
            std::vector<Component*> m_Components;
            Scene* m_Scene = nullptr;
        };
    }
}

#endif //COURSEWORK_GAMEENGINE_GAMEOBJECT_H
