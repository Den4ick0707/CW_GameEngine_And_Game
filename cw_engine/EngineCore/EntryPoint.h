#ifndef COURSEWORK_GAMEENGINE_ENTRYPOINT_H
#define COURSEWORK_GAMEENGINE_ENTRYPOINT_H

#define STB_IMAGE_IMPLEMENTATION

// Ми кажемо: "Десь у клієнті буде функція, яка створить Application"
extern Engine::Application *Engine::CreateApplication();

int main(int argc, char **argv) {
    // 1. Ініціалізація (логерів, пам'яті тощо)
    // Engine::Log::Init();

    // 2. Створення програми (викликається твоя функція з main.cpp)
    auto app = Engine::CreateApplication();

    // 3. Запуск циклу
    app->Run();

    // 4. Очищення
    delete app;

    return 0;
}
#endif //COURSEWORK_GAMEENGINE_ENTRYPOINT_H
