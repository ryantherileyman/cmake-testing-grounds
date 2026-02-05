
#include <chrono>

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#endif

#include <SDL_stdinc.h>
#include <SDL_timer.h>
#include <SDL_video.h>
#include <SDL_events.h>
#include <SDL_main.h>
#include <SDL_log.h>
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

constexpr int GAME_IMAGE_FORMATS = IMG_INIT_PNG | IMG_INIT_JPG;
constexpr int GAME_MIXER_FORMATS = MIX_INIT_OGG;

static InitSdlResult initializeSdl() {
	InitSdlResult result;

	result.sdlInitResult = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	result.sdlImageInitResult = IMG_Init(GAME_IMAGE_FORMATS);
	result.sdlTtfInitResult = TTF_Init();

	result.sdlMixInitResult = Mix_Init(GAME_MIXER_FORMATS);
	result.sdlMixOpenAudioResult = Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

	// If you're targeting a mobile build, this tells SDL to generate synthetic mouse events whenever touch gestures occur
	// This already happens for native dektop, and for web-based Emscripten builds
#if defined(__ANDROID__) || defined(__IPHONEOS__)
	SDL_SetHint(SDL_HINT_TOUCH_MOUSE_EVENTS, "1");
#endif

	return result;
}

static bool sdlInitialized(const InitSdlResult& initResult) {
	bool result =
		(initResult.sdlInitResult == 0) &&
		((initResult.sdlImageInitResult & GAME_IMAGE_FORMATS) == GAME_IMAGE_FORMATS) &&
		(initResult.sdlTtfInitResult == 0) &&
		((initResult.sdlMixInitResult & GAME_MIXER_FORMATS) == GAME_MIXER_FORMATS) &&
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

class GameTimer {

private:
	Uint64 lastCounter;
	double frequency;

public:
	GameTimer() {
		this->lastCounter = SDL_GetPerformanceCounter();
		this->frequency = static_cast<double>(SDL_GetPerformanceFrequency());
	}

	std::chrono::duration<double> tick() {
		Uint64 now = SDL_GetPerformanceCounter();
		Uint64 delta = now - this->lastCounter;

		this->lastCounter = now;

		// Duration is measured in seconds
		std::chrono::duration<double> result = std::chrono::duration<double>(delta / this->frequency);
		return result;
	}

};

struct GameUserData {
	GameTimer timer;
	bool gameIsRunning = true;
	GopherGame game;
};

#ifdef __EMSCRIPTEN__
EM_BOOL em_resize_callback(int eventType, const EmscriptenUiEvent* uiEvent, void* userData) {
	GameUserData* gameUserData = static_cast<GameUserData*>(userData);

	int canvasWidth = 0, canvasHeight = 0;
	emscripten_get_canvas_element_size("#canvas", &canvasWidth, &canvasHeight);

	SDL_SetWindowSize(gameUserData->game.getWindow(), canvasWidth, canvasHeight);

	SDL_Event event;
	SDL_zero(event);
	event.type = SDL_WINDOWEVENT;
	event.window.event = SDL_WINDOWEVENT_SIZE_CHANGED;
	event.window.data1 = canvasWidth;
	event.window.data2 = canvasHeight;
	SDL_PushEvent(&event);

	return EM_TRUE;
}
#endif

static void gameLoop(void* userData) {
	SDL_Event event;
	GameUserData* gameUserData = static_cast<GameUserData*>(userData);

	std::chrono::duration<double> timerDelta = gameUserData->timer.tick();

	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT) {
			gameUserData->gameIsRunning = false;
		}
		else {
			gameUserData->game.handleEvent(event);
		}
	}

	if (gameUserData->gameIsRunning) {
		gameUserData->game.updateState(timerDelta);
		gameUserData->game.render();
	}
}

static bool runGame() {
	// Scope the game object here, to ensure it loses scope before calling shutdownSdl()
	GameUserData gameUserData;

	bool result = gameUserData.game.isInitialized();
	if (result) {
#ifdef __EMSCRIPTEN__
		emscripten_set_resize_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, &gameUserData, false, em_resize_callback);
		emscripten_set_main_loop_arg(gameLoop, &gameUserData, 0, true);
#else
		while (gameUserData.gameIsRunning) {
			gameLoop(&gameUserData);
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
			SDL_Log("Failed to initialize the game");
			result = -1;
		}
	}
	else {
		SDL_Log("Failed to initialize SDL");
		result = -1;
	}

	shutdownSdl();

	return result;
}
