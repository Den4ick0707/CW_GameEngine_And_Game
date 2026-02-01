#include "pch.h"
#include "Input.h"


namespace Engine {
    namespace Core {
        GLFWwindow *Input::s_Window = nullptr;

        void Input::Init(GLFWwindow *window) {
            s_Window = window;
        }

        bool Input::GetKey(int keycode) {
            auto state = glfwGetKey(s_Window, keycode);
            return state == GLFW_PRESS || state == GLFW_REPEAT;
        }

        bool Input::GetMouseButton(int button) {
            auto state = glfwGetMouseButton(s_Window, button);
            return state == GLFW_PRESS;
        }

        std::pair<float, float> Input::GetMousePosition() {
            double xpos, ypos;
            glfwGetCursorPos(s_Window, &xpos, &ypos);
            return {(float) xpos, (float) ypos};
        }

        float Input::GetMouseX() {
            return GetMousePosition().first;
        }

        float Input::GetMouseY() {
            return GetMousePosition().second;
        }
    }
}
