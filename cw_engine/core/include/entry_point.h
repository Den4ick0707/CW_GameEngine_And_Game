#ifndef CW_ENGINE_CORE_ENTRYPOINT_H
#define CW_ENGINE_CORE_ENTRYPOINT_H

#include "application.h"

#include "logger.h"

int main(int argc, char** argv) {

    auto app = Engine::Core::CreateApplication();

    app->Run();

    delete app;

    return 0;
}

#endif // CW_ENGINE_CORE_ENTRYPOINT_H