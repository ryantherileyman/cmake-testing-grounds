
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)

set(SAMPLE_SDL2_EMSCRIPTEN_SOURCES
	GopherGame.cpp
	main.cpp
)

add_executable(
	Sdl2EmscriptenExample
	${SAMPLE_SDL2_EMSCRIPTEN_SOURCES}
)

target_include_directories(Sdl2EmscriptenExample PUBLIC "${PROJECT_SOURCE_DIR}/include")

if(NOT EMSCRIPTEN)
	message(STATUS "Configuring native desktop build")

	if(TARGET SDL2::SDL2main)
		target_link_libraries(
			Sdl2EmscriptenExample PRIVATE
			SDL2::SDL2main
		)
	endif()
	target_link_libraries(
		Sdl2EmscriptenExample PRIVATE
		SDL2::SDL2-static
		SDL2_image::SDL2_image-static
		SDL2_mixer::SDL2_mixer-static
		SDL2_ttf::SDL2_ttf-static
	)

	add_custom_target(
		CopySdl2EmscriptenExampleResources ALL
		COMMAND ${CMAKE_COMMAND} -E rm -rf
			"$<TARGET_FILE_DIR:Sdl2EmscriptenExample>/resources"
		COMMAND ${CMAKE_COMMAND} -E copy_directory
			"${CMAKE_SOURCE_DIR}/src/resources"
			"$<TARGET_FILE_DIR:Sdl2EmscriptenExample>/resources"
	)
	add_dependencies(Sdl2EmscriptenExample CopySdl2EmscriptenExampleResources)
endif()
