#include "input.h"
#include <GLFW/glfw3.h>

namespace Engine::Core {

    Input& Input::Get() {
        static Input instance;
        return instance;
    }

    void Input::BindAction(const std::string& name, KeyCode key) {
        int k = static_cast<int>(key);
        m_KeyBindings[name] = k;
        m_KeyStates.emplace(k, GLFW_RELEASE);
    }

    void Input::BindAction(const std::string& name, MouseCode btn) {
        int b = static_cast<int>(btn);
        m_MouseBindings[name] = b;
        m_MouseStates.emplace(b, GLFW_RELEASE);
    }

    void Input::Subscribe(const std::string& name, ActionCallback cb) {
        m_Callbacks[name].push_back(std::move(cb));
    }

    void Input::ClearSubscriptions() { m_Callbacks.clear(); }

    bool Input::IsKeyHeld(KeyCode key) const {
        auto it = m_KeyStates.find(static_cast<int>(key));
        return it != m_KeyStates.end() &&
               (it->second == GLFW_PRESS || it->second == GLFW_REPEAT);
    }

    bool Input::IsMouseHeld(MouseCode btn) const {
        auto it = m_MouseStates.find(static_cast<int>(btn));
        return it != m_MouseStates.end() && it->second == GLFW_PRESS;
    }

    void Input::Dispatch(const std::string& name) {
        auto it = m_Callbacks.find(name);
        if (it == m_Callbacks.end()) return;
        for (const auto& fn : it->second)
            if (fn) fn();
    }

    void Input::Update() {
        GLFWwindow* win = glfwGetCurrentContext();
        if (!win) return;

        for (auto& [name, key] : m_KeyBindings) {
            int cur = glfwGetKey(win, key);
            int old = m_KeyStates.count(key) ? m_KeyStates[key] : GLFW_RELEASE;
            if (cur == GLFW_PRESS && old == GLFW_RELEASE)
                Dispatch(name);
            m_KeyStates[key] = cur;
        }

        for (auto& [name, btn] : m_MouseBindings) {
            int cur = glfwGetMouseButton(win, btn);
            int old = m_MouseStates.count(btn) ? m_MouseStates[btn] : GLFW_RELEASE;
            if (cur == GLFW_PRESS && old == GLFW_RELEASE)
                Dispatch(name);
            m_MouseStates[btn] = cur;
        }
    }

} // namespace Engine::Core