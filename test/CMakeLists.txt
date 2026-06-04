cmake_minimum_required(VERSION 4.0)
project(sudoku_solver C)

set(CMAKE_C_STANDARD 11)

add_executable(test_phase1
        test_phase1.c
        ../entity/grid.c
        ../entity/sudoku_types.c
        ../repository/io.c
)

target_include_directories(test_phase1 PRIVATE
        ../entity
        ../repository
        ../service
)

# Enregistre le test pour `ctest`
add_test(NAME Phase1Tests COMMAND test_phase1)
