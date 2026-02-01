#ifndef COURSEWORK_GAMEENGINE_APPLICATION_H
#define COURSEWORK_GAMEENGINE_APPLICATION_H

#include "Window.h"

namespace Engine {
    namespace Core {
        class Application {
        public:
            Application();

            virtual ~Application();

            void Run();

            virtual void OnInit() {
            }

            virtual void OnUpdate(float dt) {
            }

            virtual void OnShutdown() {
            }

            Window &GetWindow() { return *m_Window; }

            static Application &Get() { return *s_Instance; }

        private:
            std::unique_ptr<Window> m_Window;
            bool m_Running = true;
            float m_LastFrameTime = 0.0f;

            static Application *s_Instance;
        };

        Application *CreateApplication();
    }
}
#endif //COURSEWORK_GAMEENGINE_APPLICATION_H
