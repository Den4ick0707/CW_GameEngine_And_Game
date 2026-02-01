cmake_minimum_required(VERSION 4.1)
project(CourseWork_GameEngine)

# Налаштування стандартів C++
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Додаємо підпроєкти
add_subdirectory(cw_engine)
add_subdirectory(cw_game)