#pragma once

#include <string>
#include <functional>

struct GLFWwindow;

namespace Engine::Core {

    struct WindowProps {
        std::string Title  = "CW Engine";
        int         Width  = 1280;
        int         Height = 720;

        WindowProps() = default;
        WindowProps(std::string title, int w, int h)
            : Title(std::move(title)), Width(w), Height(h) {}
    };

    /// @brief RAII-обгортка над GLFW вікном і OpenGL контекстом.
    class Window {
    public:
        explicit Window(const WindowProps& props = {});
        ~Window();

        Window(const Window&)            = delete;
        Window& operator=(const Window&) = delete;

        /// @brief PollEvents + SwapBuffers.
        void Update();

        void SetVSync(bool enabled);

        [[nodiscard]] int  GetWidth()    const { return m_Data.Width;  }
        [[nodiscard]] int  GetHeight()   const { return m_Data.Height; }
        [[nodiscard]] bool IsVSync()     const { return m_Data.VSync;  }
        [[nodiscard]] bool ShouldClose() const;

        [[nodiscard]] GLFWwindow* GetNativeWindow() const { return m_Window; }

    private:
        void Init(const WindowProps& props);
        void Shutdown();

        GLFWwindow* m_Window = nullptr;

        struct WindowData {
            std::string Title;
            int  Width  = 1280;
            int  Height = 720;
            bool VSync  = false;
        } m_Data;
    };

} // namespace Engine::Core