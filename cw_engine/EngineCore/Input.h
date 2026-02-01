// Input.h
#pragma once
#include <glm/glm.hpp>

namespace Engine {
    static class Input {
    public:
        bool IsKeyPressed(GLFWwindow *s_Window, int keycode);

        bool IsMouseButtonPressed(GLFWwindow *s_Window, int button);

        glm::vec2 GetMousePosition(GLFWwindow *s_Window);
    };
}
