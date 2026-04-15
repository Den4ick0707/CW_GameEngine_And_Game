    cmake_minimum_required(VERSION 4.1.2)

    project(cw_game)

    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)

    if(CMAKE_BUILD_TYPE MATCHES Debug)
        add_compile_definitions(CW_DEBUG)
        add_compile_definitions(CW_ENABLE_LOGGING)
    endif()


    add_executable(${PROJECT_NAME} src/main.cpp src/platformer_scene.cpp)
    target_include_directories(${PROJECT_NAME} PUBLIC include)


    target_link_libraries(${PROJECT_NAME} PRIVATE cw_engine )

    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/res"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>/res"
    )
    target_precompile_headers(${PROJECT_NAME} PRIVATE pch.h)