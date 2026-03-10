cmake_minimum_required(VERSION 4.1.2)
project(cw_engine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(LIBS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs)


# GLFW
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
add_subdirectory(${LIBS_DIR}/glfw)

# GLAD
add_library(glad_lib STATIC ${LIBS_DIR}/glad/src/glad.c)
target_include_directories(glad_lib PUBLIC ${LIBS_DIR}/glad/include)

# GLM
add_library(glm_lib INTERFACE)
target_include_directories(glm_lib INTERFACE ${LIBS_DIR}/glm)

# JSON
add_library(json_lib INTERFACE)
target_include_directories(json_lib INTERFACE ${LIBS_DIR}/json/include)

# ImGui
file(GLOB IMGUI_SOURCES
        "${LIBS_DIR}/imgui/imgui.cpp"
        "${LIBS_DIR}/imgui/imgui_demo.cpp"
        "${LIBS_DIR}/imgui/imgui_draw.cpp"
        "${LIBS_DIR}/imgui/imgui_tables.cpp"
        "${LIBS_DIR}/imgui/imgui_widgets.cpp"
        "${LIBS_DIR}/imgui/backends/imgui_impl_glfw.cpp"
        "${LIBS_DIR}/imgui/backends/imgui_impl_opengl3.cpp"
)
add_library(imgui_lib STATIC ${IMGUI_SOURCES})
target_include_directories(imgui_lib PUBLIC ${LIBS_DIR}/imgui ${LIBS_DIR}/imgui/backends)
target_link_libraries(imgui_lib PUBLIC glfw glad_lib) # ImGui залежить від GLFW та OpenGL


# --- 2. ПІДКЛЮЧЕННЯ ВНУТРІШНІХ МОДУЛІВ ---

add_subdirectory(core)
add_subdirectory(graphics)
add_subdirectory(physics)
add_subdirectory(scene)


add_library(${PROJECT_NAME} INTERFACE )

target_link_libraries(${PROJECT_NAME} INTERFACE
        cw_core
        cw_graphics
        imgui_lib
        glad_lib
        glfw
        glm_lib
        json_lib
        dl
        pthread
)