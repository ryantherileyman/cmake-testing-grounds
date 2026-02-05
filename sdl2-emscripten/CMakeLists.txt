
cmake_minimum_required(VERSION 3.24)

project(Sdl2EmscriptenExample
	VERSION 1.1
	LANGUAGES CXX
)

if(EMSCRIPTEN AND NOT DEFINED ENV{EMSDK})
message(FATAL_ERROR "EMSDK environment variable is not set")
endif()

if(NOT EMSCRIPTEN)
add_subdirectory(dependencies/sdl2)
endif()

add_subdirectory(src/main)
