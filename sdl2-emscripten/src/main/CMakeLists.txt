
set(SAMPLE_SDL2_EMSCRIPTEN_SOURCES
	GopherGame.cpp
	main.cpp
)

add_executable(
	Sdl2EmscriptenExample
	${SAMPLE_SDL2_EMSCRIPTEN_SOURCES}
)

target_compile_features(
	Sdl2EmscriptenExample PRIVATE
	cxx_std_17
)

target_include_directories(
	Sdl2EmscriptenExample PRIVATE
	"${PROJECT_SOURCE_DIR}/include"
)

if(NOT EMSCRIPTEN)
	message(STATUS "Configuring native desktop build")

	if(WIN32)
		set_target_properties(
			Sdl2EmscriptenExample PROPERTIES
			WIN32_EXECUTABLE $<$<CONFIG:Release>:ON>
		)
	endif()
	
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

if(EMSCRIPTEN)
	message(STATUS "Configuring Emscripten WebAssembly build")

	set_target_properties(
		Sdl2EmscriptenExample PROPERTIES
		SUFFIX ".html"
	)

	# For a complete list of options, see https://emscripten.org/docs/tools_reference/settings_reference.html
	# Most options are linker only, but these options must be sent to the compiler as well
	target_compile_options(
		Sdl2EmscriptenExample PRIVATE
		"-sUSE_SDL=2"
		"-sUSE_SDL_IMAGE=2"
		"-sUSE_SDL_MIXER=2"
		"-sUSE_SDL_TTF=2"
		"-sSDL2_IMAGE_FORMATS=[\"png\",\"jpg\"]"
		"-sSDL2_MIXER_FORMATS=[\"ogg\"]"
	)

	target_link_options(
		Sdl2EmscriptenExample PRIVATE
		"-sUSE_SDL=2"
		"-sUSE_SDL_IMAGE=2"
		"-sUSE_SDL_MIXER=2"
		"-sUSE_SDL_TTF=2"
		"-sSDL2_IMAGE_FORMATS=[\"png\",\"jpg\"]"
		"-sSDL2_MIXER_FORMATS=[\"ogg\"]"
		"--preload-file" "${CMAKE_SOURCE_DIR}/src/resources@resources"
		"-sALLOW_MEMORY_GROWTH=1"
		"-sASSERTIONS=1"
		"-sFORCE_FILESYSTEM=1"
	)
endif()
