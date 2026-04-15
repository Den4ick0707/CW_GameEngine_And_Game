#ifndef looool
#define looool

#include "application.h"

int main(int /*argc*/, char** /*argv*/) {
    auto* app = Engine::Core::CreateApplication();
    app->Run();
    delete app;
    return 0;
}
#endif