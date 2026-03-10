#ifndef CW_ENGINE_CORE_WINDOW_H
#define CW_ENGINE_CORE_WINDOW_H

#include <string>
#include <functional>

struct GLFWwindow;

namespace Engine::Core {
    /// @brief Структура для початкових налаштувань вікна
    struct WindowProps {
        std::string Title;
        int Width;
        int Height;

        WindowProps(const std::string &title = "CW Engine",
                    int width = 1280,
                    int height = 720)
            : Title(title), Width(width), Height(height) {
        }
    };

    class Window {
    public:
        /// @brief Створює вікно з заданими параметрами
        explicit Window(const WindowProps &props = WindowProps());

        ~Window();

        /// @brief Оновлює вікно (PollEvents + SwapBuffers)
        void Update();

        /// @brief Отримати ширину вікна
        [[nodiscard]] int GetWidth() const { return m_Data.Width; }

        /// @brief Отримати висоту вікна
        [[nodiscard]] int GetHeight() const { return m_Data.Height; }

        /// @brief Увімкнути/Вимкнути вертикальну синхронізацію
        void SetVSync(bool enabled);

        /// @brief Перевірити стан VSync
        [[nodiscard]] bool IsVSync() const;

        /// @brief Отримати нативний вказівник (для ImGui або інших хаків)
        [[nodiscard]] GLFWwindow *GetNativeWindow() const { return m_Window; }

        /// @brief
        [[nodiscard]] int ShouldClose() const ;

    private:
        /// @brief Внутрішня ініціалізація
        void Init(const WindowProps &props);

        /// @brief Очищення ресурсів
        void Shutdown();

        GLFWwindow *m_Window;

        struct WindowData {
            std::string Title;
            int Width, Height;
            bool VSync;
        };

        WindowData m_Data;
    };
}

#endif //CW_ENGINE_CORE_WINDOW_H
