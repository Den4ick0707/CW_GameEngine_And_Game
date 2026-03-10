#ifndef CW_ENGINE_CORE_INPUT_H
#define CW_ENGINE_CORE_INPUT_H

#include <functional>
#include <unordered_map>
#include <string>
#include <vector>
#include <cstdint>
#include <iostream>

namespace Engine::Core {
    // ==========================================
    //               ENUMS
    // ==========================================

    /// @brief Engine-specific key codes (Matches GLFW)
    enum class KeyCode : int16_t {
        Space = 32, Apostrophe = 39, Comma = 44, Minus = 45, Period = 46, Slash = 47,
        D0 = 48, D1 = 49, D2 = 50, D3 = 51, D4 = 52, D5 = 53, D6 = 54, D7 = 55, D8 = 56, D9 = 57,
        Semicolon = 59, Equal = 61,
        A = 65, B = 66, C = 67, D = 68, E = 69, F = 70, G = 71, H = 72, I = 73, J = 74, K = 75, L = 76, M = 77,
        N = 78, O = 79, P = 80, Q = 81, R = 82, S = 83, T = 84, U = 85, V = 86, W = 87, X = 88, Y = 89, Z = 90,
        LeftBracket = 91, Backslash = 92, RightBracket = 93, GraveAccent = 96,
        Escape = 256, Enter = 257, Tab = 258, Backspace = 259, Insert = 260, Delete = 261,
        Right = 262, Left = 263, Down = 264, Up = 265,
        PageUp = 266, PageDown = 267, Home = 268, End = 269,
        CapsLock = 280, ScrollLock = 281, NumLock = 282, PrintScreen = 283, Pause = 284,
        F1 = 290, F2 = 291, F3 = 292, F4 = 293, F5 = 294, F6 = 295, F7 = 296, F8 = 297, F9 = 298, F10 = 299,
        F11 = 300, F12 = 301,
        LeftShift = 340, LeftControl = 341, LeftAlt = 342, LeftSuper = 343,
        RightShift = 344, RightControl = 345, RightAlt = 346, RightSuper = 347, Menu = 348
    };

    /// @brief Engine-specific mouse codes (Matches GLFW)
    enum class MouseCode : int16_t {
        Button0 = 0, Button1 = 1, Button2 = 2, Button3 = 3, Button4 = 4, Button5 = 5, Button6 = 6, Button7 = 7,
        ButtonLast = Button7,
        ButtonLeft = Button0,
        ButtonRight = Button1,
        ButtonMiddle = Button2
    };

    inline std::ostream &operator<<(std::ostream &os, KeyCode keyCode) {
        os << static_cast<int32_t>(keyCode);
        return os;
    }

    // ==========================================
    //               INPUT CLASS
    // ==========================================

    /// @brief Input Manager system based on Action Binding.
    class Input {
    public:
        using ActionCallback = std::function<void()>;

        static Input &Get();

        Input(const Input &) = delete;

        Input &operator=(const Input &) = delete;

        /// @brief Binds a KEYBOARD key to an action name.
        /// @param actionName Name of the action (e.g., "Jump").
        /// @param key The physical key code.
        void BindAction(const std::string &actionName, KeyCode key);

        /// @brief Binds a MOUSE button to an action name.
        /// @param actionName Name of the action (e.g., "Fire").
        /// @param button The physical mouse button code.
        void BindAction(const std::string &actionName, MouseCode button);

        /// @brief Subscribes a function to an action.
        void Subscribe(const std::string &actionName, ActionCallback callback);

        /// @brief Clears all subscriptions (call on level change).
        void ClearSubscriptions();

        /// @brief Processes input events. Call this in Application::Run loop.
        void Update();

    private:
        Input() = default;

        ~Input() = default;

        void Dispatch(const std::string &actionName);

    private:
        // Save key: name -> code
        std::unordered_map<std::string, int> m_KeyBindings;
        // Save mouse button: name -> code
        std::unordered_map<std::string, int> m_MouseBindings;

        // Subscribe: name -> function list
        std::unordered_map<std::string, std::vector<ActionCallback> > m_Callbacks;

        // State
        std::unordered_map<int, int> m_KeyStates;
        std::unordered_map<int, int> m_MouseStates;
    };
}

#endif // CW_ENGINE_CORE_INPUT_H
