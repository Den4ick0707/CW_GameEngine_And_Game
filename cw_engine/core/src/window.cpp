#include "window.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "logger.h"

namespace Engine::Core {
    static bool s_GLFWInitialized = false;

    static void GLFWErrorCallback(int error, const char *description) {
        CW_ERROR_LOG("GLFW Error ({0}): {1}", error, description);
    }

    Window::Window(const WindowProps &props) {
        Init(props);
    }

    Window::~Window() {
        Shutdown();
    }

    int Window::ShouldClose() const {
        return glfwWindowShouldClose(m_Window);
    }

    void Window::Init(const WindowProps &props) {
        m_Data.Title = props.Title;
        m_Data.Width = props.Width;
        m_Data.Height = props.Height;

        CW_INFO_LOG("Creating window {0} ({1}, {2})", props.Title, props.Width, props.Height);

        if (!s_GLFWInitialized) {
            // 1. Ініціалізація GLFW
            int success = glfwInit();
            // Тут можна кинути assert або exception, якщо не вдалося
            if (!success) {
                CW_FATAL_LOG("Could not initialize GLFW!");
                return;
            }

            glfwSetErrorCallback(GLFWErrorCallback);
            s_GLFWInitialized = true;
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

        m_Window = glfwCreateWindow((int) props.Width, (int) props.Height, m_Data.Title.c_str(), nullptr, nullptr);

        if (!m_Window) {
            CW_FATAL_LOG("Failed to create GLFW window!");
            glfwTerminate();
            return;
        }

        glfwMakeContextCurrent(m_Window);

        int status = gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
        if (!status) {
            CW_FATAL_LOG("Failed to initialize GLAD!");
            return;
        }

        CW_INFO_LOG("OpenGL Info:");
        CW_INFO_LOG("  Vendor: {0}", (const char*)glGetString(GL_VENDOR));
        CW_INFO_LOG("  Renderer: {0}", (const char*)glGetString(GL_RENDERER));
        CW_INFO_LOG("  Version: {0}", (const char*)glGetString(GL_VERSION));

        glfwSetWindowUserPointer(m_Window, &m_Data);

        SetVSync(false);

        glfwSetFramebufferSizeCallback(m_Window, [](GLFWwindow *window, int width, int height) {
            WindowData &data = *(WindowData *) glfwGetWindowUserPointer(window);
            data.Width = width;
            data.Height = height;

            glViewport(0, 0, width, height);
        });
    }

    void Window::Shutdown() {
        if (m_Window) {
            glfwDestroyWindow(m_Window);
        }
        glfwTerminate();
        s_GLFWInitialized = false;
    }

    void Window::Update() {
        glfwPollEvents();
        glfwSwapBuffers(m_Window);
    }

    void Window::SetVSync(bool enabled) {
        if (enabled)
            glfwSwapInterval(1);
        else
            glfwSwapInterval(0);

        m_Data.VSync = enabled;
    }

    bool Window::IsVSync() const {
        return m_Data.VSync;
    }
}
