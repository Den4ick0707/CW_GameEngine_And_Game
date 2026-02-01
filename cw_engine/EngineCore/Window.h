#ifndef COURSEWORK_GAMEENGINE_WINDOW_H
#define COURSEWORK_GAMEENGINE_WINDOW_H

namespace Engine {
    namespace Core {
        class Window {
        private:
            GLFWwindow *m_Window;
            int width, height;
            std::string title;

            bool InitGLFW();

            bool InitGLAD();

        public:
            Window(int width, int height, const std::string &title);

            ~Window();

            void OnUpdate();

            int GetWidth() const { return width; }
            int GetHeight() const { return height; }
            GLFWwindow *GetNativeWindow() const { return m_Window; }

            bool ShouldClose() const;

            static void FramebufferSizeCallback(GLFWwindow *window, int width, int height);
        };
    }
}


#endif //COURSEWORK_GAMEENGINE_WINDOW_H
