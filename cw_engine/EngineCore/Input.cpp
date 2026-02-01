#include "Input.h"


namespace Engine {
    namespace Core {
        bool Input::IsKeyPressed(GLFWwindow* s_Window, int keycode) {
            auto state = glfwGetKey(s_Window, keycode);
            return state == GLFW_PRESS || state == GLFW_REPEAT;
        }

        bool Input::IsMouseButtonPressed(GLFWwindow* s_Window, int button) {
            auto state = glfwGetMouseButton(s_Window, button);
            return state == GLFW_PRESS;
        }

        glm::vec2 Input::GetMousePosition(GLFWwindow* s_Window) {
            double xpos, ypos;
            glfwGetCursorPos(s_Window, &xpos, &ypos);
            return {(float) xpos, (float) ypos};
        }
    }
}
