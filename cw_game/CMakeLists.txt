project(cw_game)

# Знаходимо всі файли .cpp у папці src
file(GLOB GAME_SOURCES "src/*.cpp")

# Створюємо файл запуску з main.cpp та всіма знайденими файлами
add_executable(cw_game main.cpp ${GAME_SOURCES})

# Підключаємо наш рушій (це автоматично підтягне GLFW, GLM та інше)
target_link_libraries(cw_game PRIVATE cw_engine)

# Вказуємо грі шлях до її власних файлів та файлів рушія
target_include_directories(cw_game PRIVATE . ../cw_engine)

# Копіюємо папку Resource у папку з бінарником після збірки (важливо для завантаження текстур)
add_custom_command(TARGET cw_game POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/Resource"
        "$<TARGET_FILE_DIR:cw_game>/Resource"
)
