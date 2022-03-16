cmake_minimum_required(VERSION 3.0.0)
project(sdl2_snake VERSION 0.1.0)

add_executable(sdl2_snake src/main.c src/hamilton.c src/graphics.c)

target_link_libraries(sdl2_snake SDL2main SDL2)