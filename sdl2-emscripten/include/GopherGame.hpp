
#include <memory>
#include <chrono>
#include <SDL.h>
#pragma once

class GopherGame {

public:
	GopherGame();
	~GopherGame();

	bool isInitialized() const;

	void handleEvent(const SDL_Event& event);
	void updateState(std::chrono::duration<double> timerDelta);
	void render();

private:
	struct Impl;
	std::unique_ptr<Impl> ptr;

};
