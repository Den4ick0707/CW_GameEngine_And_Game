#ifndef CW_ENGINE_CORE_APPLICATION_H
#define CW_ENGINE_CORE_APPLICATION_H

#include <memory>
#include "window.h"

namespace Engine::Core {

    class Application {
    public:
        Application();
        virtual ~Application();

        void Run();

        virtual void Update(float dt)=0;
        virtual void Render() =0;

        static Application& Get() { return *s_Instance; }
        Window& GetWindow() { return *m_Window; }

    private:
        static Application* s_Instance;

        std::unique_ptr<Window> m_Window;

        bool m_Running = true;
    };

    Application* CreateApplication();
}

#endif // CW_ENGINE_CORE_APPLICATION_H