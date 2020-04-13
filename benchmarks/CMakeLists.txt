cmake_minimum_required(VERSION 3.7)

project(Strategy_benchmarks)

set(CMAKE_C_STANDARD 11)
add_definitions(
        -D_FORTIFY_SOURCE=2
        -O3
        -Wall
        -Werror
        -Wno-unused
)

set(SOURCES
    ../array.c
)

add_executable(array_benchmarks
    array_benchmarks.c
    ../array.c
)
target_link_libraries(array_benchmarks m)



