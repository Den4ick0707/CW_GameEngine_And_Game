cmake_minimum_required(VERSION 3.16)
project(cw_core)

file(GLOB_RECURSE SOURCES src/*.cpp)
file(GLOB_RECURSE HEADERS include/*.h)

add_library(${PROJECT_NAME} STATIC ${SOURCES} ${HEADERS})

target_include_directories(${PROJECT_NAME} PUBLIC include)

target_link_libraries(${PROJECT_NAME} PRIVATE
        glfw
        glad_lib
        glm_lib
)

target_precompile_headers(${PROJECT_NAME} PRIVATE src/core_pch.h)