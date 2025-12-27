cmake_minimum_required(VERSION 3.10)
project(balagur_fate_3_editor)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Включаем тестирование
enable_testing()

# Определяем компилятор
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
    set(CMAKE_CXX_FLAGS_RELEASE "-O2")
    set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
elseif(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS -DNOMINMAX)
    set(CMAKE_CXX_FLAGS_RELEASE "/O2 /MT")
    set(CMAKE_CXX_FLAGS_DEBUG "/Zi /MTd")
endif()

# Основной исполняемый файл
set(SOURCES
    main.cpp
    npc.cpp
    observer.cpp
    factory.cpp
    visitor.cpp
    dungeon_editor.cpp
    game_engine.cpp
)

add_executable(editor ${SOURCES})
target_include_directories(editor PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

if(MINGW OR CMAKE_COMPILER_IS_GNUCXX)
    target_link_libraries(editor pthread)
endif()

# Подключаем локальный Google Test
add_subdirectory(googletest)

# Тестовый исполняемый файл
add_executable(rpg_tests
    tests.cpp
    npc.cpp
    observer.cpp
    factory.cpp
    visitor.cpp
    dungeon_editor.cpp
    game_engine.cpp
)
target_include_directories(rpg_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Подключаем Google Test библиотеки
target_link_libraries(rpg_tests
    gtest
    gtest_main
    gmock
)

if(MINGW OR CMAKE_COMPILER_IS_GNUCXX)
    target_link_libraries(rpg_tests pthread)
endif()

# Добавляем тест в CTest
add_test(NAME RPG_Tests COMMAND rpg_tests)