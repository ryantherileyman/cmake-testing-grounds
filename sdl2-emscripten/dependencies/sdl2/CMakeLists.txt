
if (EMSCRIPTEN)
	message(FATAL_ERROR "SDL2 should not be included for Emscripten builds, which use an Emscripten port instead")
endif()

# SDL2_image, SDL2_mixer, and SDL2_ttf do not provide namespaced options to toggle between shared vs. static libraries
# This sample project will link to shared SDL2 libraries, so we toggle BUILD_SHARED_LIBS at the start and end of this script
set(SDL2_EMSCRIPTEN_EXAMPLE_BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS})

include(FetchContent)

set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON CACHE BOOL "" FORCE)
FetchContent_Declare(
	SDL2
	GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
	GIT_TAG release-2.32.10
)
FetchContent_MakeAvailable(SDL2)

set(BUILD_SHARED_LIBS OFF)
set(SDL2IMAGE_VENDORED ON CACHE BOOL "" FORCE)
set(SDL2IMAGE_AVIF OFF CACHE BOOL "" FORCE)
set(SDL2IMAGE_BMP OFF CACHE BOOL "" FORCE)
set(SDL2IMAGE_GIF OFF CACHE BOOL "" FORCE)
set(SDL2IMAGE_JPG ON CACHE BOOL "" FORCE)
set(SDL2IMAGE_JXL OFF CACHE BOOL "" FORCE)
set(SDL2IMAGE_LBM OFF CACHE BOOL "" FORCE)
set(SDL2IMAGE_PCX OFF CACHE BOOL "" FORCE)
set(SDL2IMAGE_PNG ON CACHE BOOL "" FORCE)
set(SDL2IMAGE_PNM OFF CACHE BOOL "" FORCE)
set(SDL2IMAGE_QOI OFF CACHE BOOL "" FORCE)
set(SDL2IMAGE_SVG OFF CACHE BOOL "" FORCE)
set(SDL2IMAGE_TGA OFF CACHE BOOL "" FORCE)
set(SDL2IMAGE_TIF OFF CACHE BOOL "" FORCE)
set(SDL2IMAGE_WEBP OFF CACHE BOOL "" FORCE)
set(SDL2IMAGE_XCF OFF CACHE BOOL "" FORCE)
set(SDL2IMAGE_XPM OFF CACHE BOOL "" FORCE)
set(SDL2IMAGE_XV OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
	SDL2_image
	GIT_REPOSITORY https://github.com/libsdl-org/SDL_image.git
	GIT_TAG release-2.8.8
	GIT_SUBMODULES "external/jpeg;external/libpng;external/zlib"
)
FetchContent_MakeAvailable(SDL2_image)

set(BUILD_SHARED_LIBS OFF)
set(SDL2MIXER_FLAC OFF CACHE BOOL "" FORCE)
set(SDL2MIXER_GME OFF CACHE BOOL "" FORCE)
set(SDL2MIXER_MOD OFF CACHE BOOL "" FORCE)
set(SDL2MIXER_MP3 OFF CACHE BOOL "" FORCE)
set(SDL2MIXER_MIDI OFF CACHE BOOL "" FORCE)
set(SDL2MIXER_OPUS OFF CACHE BOOL "" FORCE)
set(SDL2MIXER_WAVE OFF CACHE BOOL "" FORCE)
set(SDL2MIXER_WAVPACK OFF CACHE BOOL "" FORCE)
# Note that OGG/Vorbis support is enabled by choosing an appropriate backend
# This project uses the default STB backend; use the empty string "" if you want to turn off OGG/Vorbis support
set(SDL2MIXER_VORBIS "STB" CACHE STRING "Enable OGG Vorbis music" FORCE)
FetchContent_Declare(
	SDL2_mixer
	GIT_REPOSITORY https://github.com/libsdl-org/SDL_mixer.git
	GIT_TAG release-2.8.1
)
FetchContent_MakeAvailable(SDL2_mixer)

set(BUILD_SHARED_LIBS OFF)
set(SDL2TTF_VENDORED ON CACHE BOOL "" FORCE)
set(SDL2TTF_FREETYPE ON CACHE BOOL "" FORCE)
set(SDL2TTF_HARFBUZZ OFF CACHE BOOL "" FORCE)
set(SDL2TTF_PLUTOSVG OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
	SDL2_ttf
	GIT_REPOSITORY https://github.com/libsdl-org/SDL_ttf.git
	GIT_TAG release-2.24.0
	GIT_SUBMODULES "external/freetype"
)
FetchContent_MakeAvailable(SDL2_ttf)

set(BUILD_SHARED_LIBS ${SDL2_EMSCRIPTEN_EXAMPLE_BUILD_SHARED_LIBS})
