cmake_minimum_required(VERSION 3.21)
set(CMAKE_C_STANDARD 99)

project(DPLL)
add_executable(
        DPLL DPLL.c
        src/features.c src/features.h
        src/heuristics.h src/heuristics.c
        src/vitals.h src/vitals.c
        )
target_link_libraries(DPLL m)






