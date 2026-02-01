#include "Window.h"

namespace Engine {
    namespace Core {

        Window::Window(int w, int h, const std::string &t)
            : width(w), height(h), title(t), window(nullptr) {
            if (!InitGLFW()) {
                std::cerr << "CRITICAL ERROR: Failed to init GLFW!" << std::endl;
                // У реальному рушії тут кидають exception або assert
                exit(-1);
            }

            if (!InitGLAD()) {
                std::cerr << "CRITICAL ERROR: Failed to init GLAD!" << std::endl;
                exit(-1);
            }
        }
        Window::~Window() {
            glfwDestroyWindow(window);
            glfwTerminate();
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

            window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
            if (window == NULL) {
                std::cout << "Failed to create GLFW window" << std::endl;
                glfwTerminate();
                return false;
            }

            glfwMakeContextCurrent(window);

            // Зберігаємо вказівник на цей клас всередині вікна GLFW (знадобиться для Input пізніше)
            glfwSetWindowUserPointer(window, this);

            // Встановлюємо колбек
            glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);

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
            glfwSwapBuffers(window); // Міняє буфери (показує новий кадр)
            glfwPollEvents(); // Слухає клавіатуру/мишу
        }

        bool Window::ShouldClose() const {
            return glfwWindowShouldClose(window);
        }

        void Window::FramebufferSizeCallback(GLFWwindow *window, int width, int height) {
            glViewport(0, 0, width, height);
        }
    }
}
