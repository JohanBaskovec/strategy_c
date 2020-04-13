cmake_minimum_required(VERSION 3.7)

project(Strategy)

set(CMAKE_C_STANDARD 11)
add_definitions(
        -D_FORTIFY_SOURCE=2
        -O0
        -Wall
        -Werror
        -g
        -Wno-unused
)

find_package(SDL2 REQUIRED)
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS} /home/johan/c_project/mn_test/lib)

set(SOURCES
    maths.c
    graphics.c
    array.c
    input.c
    camera.c
    world.c
    shader.c
    default_program.c
    error.c
    mat4f.c
    vec3f.c
    fs.c
    vec4f.c
    floor.c
    sprite.c
    entity.c
    ai_component.c
)

add_executable(strategy
    main.c
    ${SOURCES}
)
target_link_libraries(strategy ${SDL2_LIBRARIES} ${OPENGL_gl_LIBRARY} ${GLEW_LIBRARIES} m)
add_executable(strategy_test
    maths_test.h
    main_test.c
    ${SOURCES}
)
target_link_libraries(strategy_test ${SDL2_LIBRARIES} ${OPENGL_gl_LIBRARY} ${GLEW_LIBRARIES} m)

