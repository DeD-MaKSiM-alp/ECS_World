cmake_minimum_required(VERSION 3.14)
project(ecs_game LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Путь к SFML — поменяйте если распаковали в другое место
set(SFML_DIR "C:/IT/cursor_projects/SFML-2.6.2/lib/cmake/SFML")
find_package(SFML 2.6 COMPONENTS graphics window system REQUIRED)

add_executable(ecs_game
    main.cpp
    src/ecs/World.cpp
    src/ecs/Systems.cpp
    src/ecs/SFMLRenderer.cpp
    src/Game.cpp
)

target_include_directories(ecs_game PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(ecs_game PRIVATE SFML::Graphics SFML::Window SFML::System)

if(MSVC)
    target_compile_options(ecs_game PRIVATE /W4 /permissive- /utf-8)
else()
    target_compile_options(ecs_game PRIVATE -Wall -Wextra -Wpedantic)
endif()
