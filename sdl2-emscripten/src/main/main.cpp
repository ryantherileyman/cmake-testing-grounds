
#include <chrono>

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>

#include "GopherGame.hpp"

struct InitSdlResult {
	int sdlInitResult = -1;
	int sdlImageInitResult = -1;
	int sdlTtfInitResult = -1;
	int sdlMixInitResult = -1;
	int sdlMixOpenAudioResult = -1;
};

static InitSdlResult initializeSdl() {
	InitSdlResult result;

	result.sdlInitResult = SDL_Init(SDL_INIT_VIDEO);
	result.sdlImageInitResult = IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);
	result.sdlTtfInitResult = TTF_Init();

	result.sdlMixInitResult = Mix_Init(MIX_INIT_OGG);
	result.sdlMixOpenAudioResult = Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

	return result;
}

static bool sdlInitialized(const InitSdlResult& initResult) {
	bool result =
		(initResult.sdlInitResult == 0) &&
		(initResult.sdlImageInitResult == (IMG_INIT_PNG | IMG_INIT_JPG)) &&
		(initResult.sdlTtfInitResult == 0) &&
		(initResult.sdlMixInitResult == MIX_INIT_OGG) &&
		(initResult.sdlMixOpenAudioResult == 0);
	return result;
}

static void shutdownSdl() {
	Mix_CloseAudio();
	Mix_Quit();

	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

class FrameTimer {

private:
	Uint64 lastCounter;
	double frequency;

public:
	FrameTimer() {
		this->lastCounter = SDL_GetPerformanceCounter();
		this->frequency = static_cast<double>(SDL_GetPerformanceFrequency());
	}

	std::chrono::duration<double> tick() {
		Uint64 now = SDL_GetPerformanceCounter();
		Uint64 delta = now - this->lastCounter;

		this->lastCounter = now;

		std::chrono::duration<double> result = std::chrono::duration<double>(delta / this->frequency);
		return result;
	}

};

FrameTimer timer;
bool gameIsRunning = true;

static void gameLoop(void* userData) {
	SDL_Event event;
	GopherGame* game = static_cast<GopherGame*>(userData);

	auto timerDelta = timer.tick();

	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT) {
			gameIsRunning = false;
		}
		else {
			game->handleEvent(event);
		}
	}

	if (gameIsRunning) {
		game->updateState(timerDelta);
		game->render();
	}
}

static bool runGame() {
	GopherGame game;

	bool result = game.isInitialized();
	if (result) {
#ifdef __EMSCRIPTEN__
		emscripten_set_main_loop_arg(gameLoop, &game, 0, true);
#else
		while (gameIsRunning) {
			gameLoop(&game);
			SDL_Delay(1);
		}
#endif
	}

	return result;
}

int main(int argc, char* argv[]) {
	int result = 0;

	InitSdlResult initSdlResult = initializeSdl();
	if (sdlInitialized(initSdlResult)) {
		if (!runGame()) {
			result = -1;
		}
	}
	else {
		result = -1;
	}

	shutdownSdl();

	return result;
}
