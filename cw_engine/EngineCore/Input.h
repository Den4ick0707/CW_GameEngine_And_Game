#pragma once
#include <glm/glm.hpp>

// Forward declaration, щоб не тягнути важкий GLFW у хедер
struct GLFWwindow;

namespace Engine {
    namespace Core {
        class Input {
        public:
            static bool IsKeyPressed(int keycode);

            static bool IsMouseButtonPressed(int button);

            static glm::vec2 GetMousePosition();

            static void Init(GLFWwindow *window);

        private:
            static GLFWwindow *s_Window;
        };
    }
}
