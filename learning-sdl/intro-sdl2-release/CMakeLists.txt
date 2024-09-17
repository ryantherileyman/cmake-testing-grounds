
cmake_minimum_required(VERSION 3.20)

project(intro-sdl2-release VERSION 1.0)

add_subdirectory(dependencies/SDL)

add_executable(
	intro-sdl2-release
	src/main.cpp
)

if ( TARGET SDL2::SDL2main )
	target_link_libraries(
		intro-sdl2-release PRIVATE
		SDL2::SDL2main
	)
endif()
target_link_libraries(
	intro-sdl2-release PRIVATE
	SDL2::SDL2
)
add_custom_command(
	TARGET intro-sdl2-release POST_BUILD COMMAND
	${CMAKE_COMMAND} -E copy_if_different
		$<TARGET_FILE:SDL2::SDL2>
		$<TARGET_FILE_DIR:intro-sdl2-release>
)
