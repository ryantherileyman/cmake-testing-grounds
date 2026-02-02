
cmake_minimum_required(VERSION 3.24)

project(Sdl2EmscriptenExample
	VERSION 1.01
	LANGUAGES CXX
)

option(EMSCRIPTEN_IDE_WORKAROUNDS "Enable IDE macro and include path hints for Emscripten builds" OFF)

if(NOT EMSCRIPTEN)
add_subdirectory(dependencies/sdl2)
endif()

add_subdirectory(src/main)
