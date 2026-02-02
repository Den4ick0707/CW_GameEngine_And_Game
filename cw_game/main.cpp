#include "src/GameApp.h"
#include <EngineCore/EntryPoint.h>

// Реалізація функції створення (вимагається EntryPoint.h)
// Ця функція створює екземпляр нашої гри.
Engine::Application* Engine::CreateApplication() {
    return new GameApp();
}
