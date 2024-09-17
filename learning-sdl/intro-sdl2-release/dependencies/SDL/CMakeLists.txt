
include(FetchContent)

FetchContent_Declare(
	SDL2
	GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
	GIT_TAG release-2.30.7
)
FetchContent_MakeAvailable(SDL2)
