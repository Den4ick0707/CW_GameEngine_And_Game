cmake_minimum_required(VERSION 3.16)
project(cw_scene)

# FIX: порожній CMakeLists.txt викликав помилку збірки.
# Поки що — пуста бібліотека-заглушка, яку можна наповнити пізніше.

file(GLOB_RECURSE SOURCES src/*.cpp)
file(GLOB_RECURSE HEADERS include/*.h)

add_library(${PROJECT_NAME} STATIC ${SOURCES} ${HEADERS})

target_include_directories(${PROJECT_NAME} PUBLIC include)

target_link_libraries(${PROJECT_NAME} PRIVATE
        cw_graphics
        glm_lib
)