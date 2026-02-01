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

            // Ініціалізація GLAD повинна бути ТУТ, одразу після створення контексту
            if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
                // "CRITICAL: GLAD Init failed!"
            }

            // Встановити колбеки подій (Resize, KeyPress)
            glfwSetWindowUserPointer(m_Window, this);
        }

        Window::~Window() {
            glfwDestroyWindow(m_Window);
            glfwTerminate(); // Обережно, якщо вікон декілька
        }

        bool Window::InitGLFW() {
            if (!glfwInit()) {
                std::cout << "GLFW init error" << std::endl;
                return false;
            }
            // Налаштування версії 3.3 Core (Золотий стандарт)
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

            // Зберігаємо вказівник на цей клас всередині вікна GLFW (знадобиться для Input пізніше)
            glfwSetWindowUserPointer(m_Window, this);

            // Встановлюємо колбек
            glfwSetFramebufferSizeCallback(m_Window, FramebufferSizeCallback);

            return true;
        }

        bool Window::InitGLAD() {
            // Завантажуємо OpenGL вказівники
            if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
                std::cout << "Failed to initialize GLAD" << std::endl;
                return false;
            }

            // Налаштовуємо в'юпорт одразу після ініціалізації
            glViewport(0, 0, width, height);

            return true;
        }

        void Window::OnUpdate() {
            glfwSwapBuffers(m_Window); // Міняє буфери (показує новий кадр)
            glfwPollEvents(); // Слухає клавіатуру/мишу
        }

        bool Window::ShouldClose() const {
            return glfwWindowShouldClose(m_Window);
        }

        void Window::FramebufferSizeCallback(GLFWwindow *window, int width, int height) {
            glViewport(0, 0, width, height);
        }
    }
}
