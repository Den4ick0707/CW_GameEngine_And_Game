#ifndef COURSEWORK_GAMEENGINE_INPUT_H
#define COURSEWORK_GAMEENGINE_INPUT_H

namespace Engine {
    namespace Core {
        class Input {
        public:
            static void Init(GLFWwindow *window);

            static bool GetKey(int keycode);

            static bool GetMouseButton(int button);

            static std::pair<float, float> GetMousePosition();

            static float GetMouseX();

            static float GetMouseY();

        private:
            static GLFWwindow *s_Window;
        };
    }
}

#endif //COURSEWORK_GAMEENGINE_INPUT_H
