cmake_minimum_required(VERSION 4.1.2)

project(cw_game)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${PROJECT_NAME}  src/main.cpp )
target_include_directories(${PROJECT_NAME} PUBLIC include)

target_link_libraries(${PROJECT_NAME} PRIVATE cw_engine)

#add_custom_command(TARGET cw_game POST_BUILD
#        COMMAND ${CMAKE_COMMAND} -E copy_directory
#        "${CMAKE_CURRENT_SOURCE_DIR}/Resource"
#        "$<TARGET_FILE_DIR:cw_game>/Resource"
#)