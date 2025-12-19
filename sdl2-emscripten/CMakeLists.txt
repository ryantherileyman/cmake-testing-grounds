
cmake_minimum_required(VERSION 3.24)

project(Sdl2EmscriptenExample VERSION 1.0)

if(NOT EMSCRIPTEN)
add_subdirectory(dependencies/sdl2)
endif()

add_subdirectory(src/main)
