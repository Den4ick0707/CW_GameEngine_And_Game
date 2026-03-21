#include "time_manager.h"
#include <GLFW/glfw3.h>

namespace Engine::Core {

    double Time::m_LastFrameTime = 0.0;
    double Time::m_DeltaTime     = 0.0;
    double Time::m_Time          = 0.0;

    void Time::Init() {
        m_LastFrameTime = glfwGetTime();
        m_Time          = m_LastFrameTime;
    }

    void Time::Update() {
        const double cur = glfwGetTime();
        m_DeltaTime      = cur - m_LastFrameTime;
        m_LastFrameTime  = cur;
        m_Time           = cur;

        // Захист від великого стрибка (брейкпоінт / лаг)
        if (m_DeltaTime > 0.1) m_DeltaTime = 0.1;
    }

} // namespace Engine::Core