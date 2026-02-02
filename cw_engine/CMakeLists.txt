project(cw_engine)

# 1. Налаштування GLFW
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
add_subdirectory(EngineLibraries/glfw)

# 2. Збірка GLAD
add_library(glad EngineLibraries/glad/src/glad.c)
target_include_directories(glad PUBLIC EngineLibraries/glad/include)

# 3. Налаштування GLM (header-only)
add_library(glm_lib INTERFACE)
target_include_directories(glm_lib INTERFACE EngineLibraries/glm)

# 4. Налаштування ImGui
set(IMGUI_SOURCES
        EngineLibraries/imgui/imgui.cpp
        EngineLibraries/imgui/imgui_demo.cpp
        EngineLibraries/imgui/imgui_draw.cpp
        EngineLibraries/imgui/imgui_tables.cpp
        EngineLibraries/imgui/imgui_widgets.cpp
        EngineLibraries/imgui/backends/imgui_impl_glfw.cpp
        EngineLibraries/imgui/backends/imgui_impl_opengl3.cpp
)

# 5. Основні файли рушія
set(ENGINE_SOURCES
        EngineCore/Application.cpp
        EngineCore/EngineCore.cpp
        EngineCore/Input.cpp
        EngineCore/Window.cpp

        Graphics/IndexBuffer.cpp
        Graphics/OrthographicCamera.cpp
        Graphics/Renderer2D.cpp
        Graphics/ShaderProgram.cpp
        Graphics/ShaderModule.cpp
        Graphics/Textures.cpp
        Graphics/VertexArray.cpp
        Graphics/VertexBuffer.cpp

        Physics/RigidBody2D.cpp

        Scene/Scene.cpp
)

# Створюємо статичну бібліотеку
add_library(cw_engine STATIC ${ENGINE_SOURCES} ${IMGUI_SOURCES})

# 6. Precompiled Header
target_compile_definitions(cw_engine PUBLIC IMGUI_DEFINE_MATH_OPERATORS)
target_precompile_headers(cw_engine PUBLIC pch.h)

# Вказуємо шляхи пошуку хедерів
target_include_directories(cw_engine PUBLIC
        .
        EngineLibraries/imgui
        EngineLibraries/imgui/backends
)

# Лінкуємо бібліотеки
target_link_libraries(cw_engine PUBLIC glfw glad glm_lib dl pthread)
