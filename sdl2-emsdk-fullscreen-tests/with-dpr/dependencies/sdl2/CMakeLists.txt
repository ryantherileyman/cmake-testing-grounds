
if (EMSCRIPTEN)
	message(FATAL_ERROR "SDL2 should not be included for Emscripten builds, which use an Emscripten port instead")
endif()

# SDL2_ttf does not provide namespaced options to toggle between shared vs. static libraries
# This sample project will link to shared SDL2 libraries, so we toggle BUILD_SHARED_LIBS at the start and end of this script
set(SDL2_MOBILE_WEB_EXAMPLE_BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS})

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

set(BUILD_SHARED_LIBS ${SDL2_MOBILE_WEB_EXAMPLE_BUILD_SHARED_LIBS})
