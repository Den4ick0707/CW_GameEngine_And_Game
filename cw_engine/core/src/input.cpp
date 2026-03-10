#include "input.h"
#include <GLFW/glfw3.h>

namespace Engine::Core {

    Input& Input::Get() {
        static Input instance;
        return instance;
    }

    void Input::BindAction(const std::string& actionName, KeyCode key) {
        int glfwKey              = static_cast<int>(key);
        m_KeyBindings[actionName] = glfwKey;
        m_KeyStates[glfwKey]      = GLFW_RELEASE;
    }

    void Input::BindAction(const std::string& actionName, MouseCode button) {
        int glfwButton                = static_cast<int>(button);
        m_MouseBindings[actionName]   = glfwButton;
        m_MouseStates[glfwButton]     = GLFW_RELEASE;
    }

    void Input::Subscribe(const std::string& actionName, ActionCallback callback) {
        m_Callbacks[actionName].push_back(std::move(callback));
    }

    void Input::ClearSubscriptions() {
        m_Callbacks.clear();
    }

    void Input::Dispatch(const std::string& actionName) {
        auto it = m_Callbacks.find(actionName);
        if (it == m_Callbacks.end()) return;

        for (const auto& func : it->second) {
            if (func) func();
        }
    }

    void Input::Update() {
        auto* window = glfwGetCurrentContext();
        if (!window) return;

        for (auto& [name, key] : m_KeyBindings) {
            int newState = glfwGetKey(window, key);

            // FIX: безпечний пошук — явно ініціалізуємо стан якщо ключ новий
            auto stateIt = m_KeyStates.find(key);
            int oldState = (stateIt != m_KeyStates.end()) ? stateIt->second : GLFW_RELEASE;

            if (newState == GLFW_PRESS && oldState == GLFW_RELEASE) {
                Dispatch(name);
            }

            m_KeyStates[key] = newState;
        }

        for (auto& [name, button] : m_MouseBindings) {
            int newState = glfwGetMouseButton(window, button);

            auto stateIt = m_MouseStates.find(button);
            int oldState = (stateIt != m_MouseStates.end()) ? stateIt->second : GLFW_RELEASE;

            if (newState == GLFW_PRESS && oldState == GLFW_RELEASE) {
                Dispatch(name);
            }

            m_MouseStates[button] = newState;
        }
    }
}