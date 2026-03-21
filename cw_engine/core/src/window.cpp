#include "window.h"
#include "logger.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Engine::Core {

    static uint32_t s_WindowCount     = 0;
    static bool     s_GLFWInitialized = false;

    static void GLFWErrorCallback(int error, const char* desc) {
        CW_ERROR_LOG("GLFW Error (%d): %s", error, desc);
    }

    Window::Window(const WindowProps& props) { Init(props); }
    Window::~Window() { Shutdown(); }

    bool Window::ShouldClose() const {
        return m_Window && glfwWindowShouldClose(m_Window);
    }

    void Window::Init(const WindowProps& props) {
        m_Data.Title  = props.Title;
        m_Data.Width  = props.Width;
        m_Data.Height = props.Height;

        if (!s_GLFWInitialized) {
            if (!glfwInit()) {
                CW_FATAL_LOG("Failed to initialize GLFW!");
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

        m_Window = glfwCreateWindow(
            props.Width, props.Height,
            props.Title.c_str(), nullptr, nullptr);

        if (!m_Window) {
            CW_FATAL_LOG("Failed to create GLFW window!");
            return;
        }
        ++s_WindowCount;

        glfwMakeContextCurrent(m_Window);

        if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
            CW_FATAL_LOG("Failed to initialize GLAD!");
            return;
        }

        CW_INFO_LOG("OpenGL %s | %s",
            reinterpret_cast<const char*>(glGetString(GL_VERSION)),
            reinterpret_cast<const char*>(glGetString(GL_RENDERER)));

        glfwSetWindowUserPointer(m_Window, &m_Data);
        SetVSync(false);

        glfwSetFramebufferSizeCallback(m_Window,
            [](GLFWwindow* w, int width, int height) {
                auto& data    = *static_cast<WindowData*>(glfwGetWindowUserPointer(w));
                data.Width    = width;
                data.Height   = height;
                glViewport(0, 0, width, height);
            });
    }

    void Window::Shutdown() {
        if (m_Window) {
            glfwDestroyWindow(m_Window);
            m_Window = nullptr;
            if (--s_WindowCount == 0) {
                glfwTerminate();
                s_GLFWInitialized = false;
            }
        }
    }

    void Window::Update() {
        glfwPollEvents();
        glfwSwapBuffers(m_Window);
    }

    void Window::SetVSync(bool enabled) {
        glfwSwapInterval(enabled ? 1 : 0);
        m_Data.VSync = enabled;
    }

} // namespace Engine::Core