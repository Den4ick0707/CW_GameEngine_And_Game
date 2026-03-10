#include "time_manager.h"

namespace Engine::Core {
    double Time::m_LastFrameTime = 0.0;
    double Time::m_DeltaTime = 0.0;
    double Time::m_Time = 0.0;

    void Time::Init() {
        m_LastFrameTime = glfwGetTime();
        m_Time = m_LastFrameTime;
    }

    void Time::Update() {
        const double current = glfwGetTime();
        m_DeltaTime = current - m_LastFrameTime;
        m_LastFrameTime = current;
        m_Time = current;

        if (m_DeltaTime > 0.1) m_DeltaTime = 0.1;
    }
}
