#include "input.h"

#include <GLFW/glfw3.h>

namespace Engine::Core {
    Input &Input::Get() {
        static Input instance;
        return instance;
    }

    void Input::BindAction(const std::string &actionName, KeyCode key) {
        int glfwKey = static_cast<int>(key);
        m_KeyBindings[actionName] = glfwKey;
        m_KeyStates[glfwKey] = GLFW_RELEASE;
    }

    void Input::BindAction(const std::string &actionName, MouseCode button) {
        int glfwButton = static_cast<int>(button);
        m_MouseBindings[actionName] = glfwButton;
        m_MouseStates[glfwButton] = GLFW_RELEASE;
    }

    void Input::Subscribe(const std::string &actionName, ActionCallback callback) {
        m_Callbacks[actionName].push_back(callback);
    }

    void Input::ClearSubscriptions() {
        m_Callbacks.clear();
    }

    void Input::Dispatch(const std::string &actionName) {
        if (m_Callbacks.find(actionName) != m_Callbacks.end()) {
            for (const auto &func: m_Callbacks[actionName]) {
                if (func) func();
            }
        }
    }

    void Input::Update() {
        auto *window = glfwGetCurrentContext();
        if (!window) return;

        for (auto &[name, key]: m_KeyBindings) {
            int state = glfwGetKey(window, key);
            int oldState = m_KeyStates[key];

            if (state == GLFW_PRESS && oldState == GLFW_RELEASE) {
                Dispatch(name);
            }

            m_KeyStates[key] = state;
        }

        for (auto &[name, button]: m_MouseBindings) {
            int state = glfwGetMouseButton(window, button);
            int oldState = m_MouseStates[button];

            if (state == GLFW_PRESS && oldState == GLFW_RELEASE) {
                Dispatch(name);
            }
            m_MouseStates[button] = state;
        }
    }
}
