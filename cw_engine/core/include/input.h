#pragma once

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>
#include <cstdint>

namespace Engine::Core {

    /// @brief GLFW-сумісні коди клавіш.
    enum class KeyCode : int16_t {
        Space=32, Apostrophe=39, Comma=44, Minus=45, Period=46, Slash=47,
        D0=48,D1,D2,D3,D4,D5,D6,D7,D8,D9,
        Semicolon=59, Equal=61,
        A=65,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,
        LeftBracket=91, Backslash=92, RightBracket=93, GraveAccent=96,
        Escape=256, Enter, Tab, Backspace, Insert, Delete,
        Right, Left, Down, Up,
        PageUp=266, PageDown, Home, End,
        CapsLock=280, ScrollLock, NumLock, PrintScreen, Pause,
        F1=290,F2,F3,F4,F5,F6,F7,F8,F9,F10,F11,F12,
        LeftShift=340, LeftControl, LeftAlt, LeftSuper,
        RightShift=344, RightControl, RightAlt, RightSuper, Menu
    };

    /// @brief GLFW-сумісні коди кнопок миші.
    enum class MouseCode : int16_t {
        Left=0, Right=1, Middle=2,
        Button3=3, Button4=4, Button5=5, Button6=6, Button7=7
    };

    /// @brief Singleton менеджер вводу на базі Action Binding.
    ///
    /// Використання:
    /// @code
    ///   Input::Get().BindAction("Jump", KeyCode::Space);
    ///   Input::Get().Subscribe("Jump", []{ player.Jump(); });
    ///
    ///   // У Update:
    ///   if (Input::Get().IsKeyHeld(KeyCode::A)) player.MoveLeft(dt);
    /// @endcode
    class Input {
    public:
        using ActionCallback = std::function<void()>;

        static Input& Get();

        Input(const Input&)            = delete;
        Input& operator=(const Input&) = delete;

        // ── Прив'язка дій ─────────────────────────────────────────────────

        void BindAction(const std::string& name, KeyCode   key);
        void BindAction(const std::string& name, MouseCode btn);

        /// @brief Підписатися на подію OnPress (спрацьовує один раз при натисканні).
        void Subscribe(const std::string& name, ActionCallback cb);

        /// @brief Очистити підписки (викликай при зміні сцени).
        void ClearSubscriptions();

        // ── Прямий запит стану ────────────────────────────────────────────

        /// @return true поки клавіша утримується.
        [[nodiscard]] bool IsKeyHeld  (KeyCode   key) const;
        [[nodiscard]] bool IsMouseHeld(MouseCode btn) const;

        // ── Оновлення (викликається Application) ─────────────────────────

        void Update();

    private:
        Input()  = default;
        ~Input() = default;

        void Dispatch(const std::string& name);

        std::unordered_map<std::string, int>                      m_KeyBindings;
        std::unordered_map<std::string, int>                      m_MouseBindings;
        std::unordered_map<std::string, std::vector<ActionCallback>> m_Callbacks;
        std::unordered_map<int, int>                              m_KeyStates;
        std::unordered_map<int, int>                              m_MouseStates;
    };

} // namespace Engine::Core