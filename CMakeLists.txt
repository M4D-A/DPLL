cmake_minimum_required(VERSION 3.21)
set(CMAKE_C_STANDARD 99)

project(DPLL)
add_executable(DPLL DPLL.c features.c features.h heuristics.h heuristics.c vitals.h vitals.c)
target_link_libraries(DPLL m)






