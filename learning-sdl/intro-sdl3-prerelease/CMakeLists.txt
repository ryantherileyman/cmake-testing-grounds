
cmake_minimum_required(VERSION 3.20)

project(intro-sdl3-prerelease VERSION 1.0)

add_subdirectory(external/SDL)

add_executable(
	intro-sdl3-prerelease
	src/main.cpp
)

target_link_libraries(
	intro-sdl3-prerelease PRIVATE
	SDL3::SDL3
)
add_custom_command(
	TARGET intro-sdl3-prerelease POST_BUILD COMMAND
	${CMAKE_COMMAND} -E copy_if_different
		$<TARGET_FILE:SDL3::SDL3>
		$<TARGET_FILE_DIR:intro-sdl3-prerelease>
)
