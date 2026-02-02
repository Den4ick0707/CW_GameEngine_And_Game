#pragma once
#include <glm/glm.hpp>

// Forward declaration
struct GLFWwindow;

namespace Engine {

    class Input {
    public:
        // Цей метод ми маємо викликати при старті програми!
        static void Init(GLFWwindow* window);

        static bool IsKeyPressed(int keycode);
        static bool IsMouseButtonPressed(int button);
        static glm::vec2 GetMousePosition();

    private:
        static GLFWwindow* s_Window;
    };
}