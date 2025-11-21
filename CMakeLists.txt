cmake_minimum_required(VERSION 3.14)
project(FixedBlockStack)

# Установка C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Компиляторные флаги
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

# Поиск Google Test
find_package(GTest REQUIRED)
include(GoogleTest)  # <-- Эта строка нужна для gtest_discover_tests

# Основная программа
add_executable(stack_main 
  src/main.cpp
)

# Тесты
add_executable(stack_test 
  tests.cpp
)

# Линковка тестов с Google Test
target_link_libraries(stack_test 
  GTest::gtest 
  GTest::gtest_main
)

# Включение заголовочных файлов
target_include_directories(stack_main PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(stack_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Автоматическое обнаружение тестов
gtest_discover_tests(stack_test)

# Тестирование
enable_testing()
