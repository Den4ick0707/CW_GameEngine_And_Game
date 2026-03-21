#pragma once

#include "application.h"
#include "logger.h"

/// @brief Точка входу рушія. Включай лише в одному .cpp файлі (main.cpp).
int main(int /*argc*/, char** /*argv*/) {
    auto* app = Engine::Core::CreateApplication();
    app->Run();
    delete app;
    return 0;
}