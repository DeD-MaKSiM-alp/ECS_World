cmake_minimum_required(VERSION 3.14)
project(ecs_game LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Автопоиск SFML для разных машин:
# 1) SFML_DIR (переменная CMake/окружения),
# 2) SFML_ROOT (переменная окружения),
# 3) локальный путь рядом с проектом,
# 4) старый путь для обратной совместимости.
if(NOT DEFINED SFML_DIR)
    set(_sfml_candidate_dirs
        "$ENV{SFML_DIR}"
        "$ENV{SFML_ROOT}/lib/cmake/SFML"
        "${CMAKE_CURRENT_SOURCE_DIR}/SFML-2.6.2/lib/cmake/SFML"
        "C:/IT/cursor_projects/SFML-2.6.2/lib/cmake/SFML"
    )

    foreach(_sfml_dir IN LISTS _sfml_candidate_dirs)
        if(_sfml_dir AND EXISTS "${_sfml_dir}/SFMLConfig.cmake")
            set(SFML_DIR "${_sfml_dir}" CACHE PATH "Path to SFMLConfig.cmake directory" FORCE)
            break()
        endif()
    endforeach()
endif()

if(NOT DEFINED SFML_DIR OR NOT EXISTS "${SFML_DIR}/SFMLConfig.cmake")
    message(FATAL_ERROR
        "SFML not found. Set SFML_DIR to a folder containing SFMLConfig.cmake "
        "(for example, <SFML>/lib/cmake/SFML), or set SFML_ROOT."
    )
endif()

find_package(SFML 2.6 COMPONENTS graphics window system REQUIRED)

add_executable(ecs_game
    main.cpp
    src/SmokeTests.cpp
    src/ecs/World.cpp
    src/ecs/Systems.cpp
    src/ecs/SFMLRenderer.cpp
    src/Game.cpp
)

target_include_directories(ecs_game PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(ecs_game PRIVATE sfml-graphics sfml-window sfml-system)

if(WIN32)
    add_custom_command(TARGET ecs_game POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:sfml-graphics>
            $<TARGET_FILE:sfml-window>
            $<TARGET_FILE:sfml-system>
            $<TARGET_FILE_DIR:ecs_game>
    )
endif()

if(MSVC)
    target_compile_options(ecs_game PRIVATE /W4 /permissive- /utf-8)
else()
    target_compile_options(ecs_game PRIVATE -Wall -Wextra -Wpedantic)
endif()
