cmake_minimum_required(VERSION 3.14)
project(ecs_game LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(ecs_game
    main.cpp
    src/ecs/World.cpp
)

target_include_directories(ecs_game PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")

if(MSVC)
    target_compile_options(ecs_game PRIVATE /W4 /permissive-)
else()
    target_compile_options(ecs_game PRIVATE -Wall -Wextra -Wpedantic)
endif()
