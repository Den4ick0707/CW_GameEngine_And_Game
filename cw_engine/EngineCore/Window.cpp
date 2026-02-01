#include "Window.h"

namespace Engine {
    namespace Core {
        Window::Window(int width, int height, const std::string &title) {
            if (!glfwInit()) {
                // Логування помилки! "CRITICAL: GLFW Init failed!"
                return;
            }

            m_Window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
            glfwMakeContextCurrent(m_Window);

            if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
                // "CRITICAL: GLAD Init failed!"
            }

            glfwSetWindowUserPointer(m_Window, this);
        }

        Window::~Window() {
            glfwDestroyWindow(m_Window);
            glfwTerminate();
        }

        bool Window::InitGLFW() {
            if (!glfwInit()) {
                std::cout << "GLFW init error" << std::endl;
                return false;
            }
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

            m_Window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
            if (m_Window == NULL) {
                std::cout << "Failed to create GLFW window" << std::endl;
                glfwTerminate();
                return false;
            }

            glfwMakeContextCurrent(m_Window);

            glfwSetWindowUserPointer(m_Window, this);

            glfwSetFramebufferSizeCallback(m_Window, FramebufferSizeCallback);

            return true;
        }

        bool Window::InitGLAD() {
            if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
                std::cout << "Failed to initialize GLAD" << std::endl;
                return false;
            }

            glViewport(0, 0, width, height);

            return true;
        }

        void Window::OnUpdate() {
            glfwSwapBuffers(m_Window);
            glfwPollEvents();
        }

        bool Window::ShouldClose() const {
            return glfwWindowShouldClose(m_Window);
        }

        void Window::FramebufferSizeCallback(GLFWwindow *window, int width, int height) {
            glViewport(0, 0, width, height);
        }
    }
}
