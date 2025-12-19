
#include <string>
#include <random>
#include <cmath>
#include <stdexcept>

#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <SDL_image.h>

#include "GopherGame.hpp"

enum class GopherGameState {
	WAIT_TO_START,
	PLAY_GAME,
};

enum class GopherAppearanceState {
	WAITING,
	SCHEDULED,
	APPEARED,
	HIT,
};

struct GopherGame::Impl {

	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;

	TTF_Font* font = nullptr;
	Mix_Music* music = nullptr;

	SDL_Texture* gopherTexture = nullptr;
	SDL_Texture* gopherHitTexture = nullptr;
	SDL_Texture* attributionTexture = nullptr;

	Mix_Chunk* gopherAppearSfx = nullptr;
	Mix_Chunk* gopherHitSfx = nullptr;
	Mix_Chunk* gopherDisappearSfx = nullptr;

	GopherGameState gameState = GopherGameState::WAIT_TO_START;
	std::mt19937 prng{ std::random_device{}() }; // Could be marked "mutable"
	int hitCount = 0;
	int missCount = 0;

	GopherAppearanceState gopherAppearanceState = GopherAppearanceState::WAITING;
	SDL_Point gopherPosition;
	double secondsUntilAppearance = 0.0f;
	double secondsUntilDisappearance = 0.0f;

	Impl() {
		this->window = this->createWindow();
		if (this->window != nullptr) {
			this->renderer = SDL_CreateRenderer(this->window, -1, SDL_RENDERER_ACCELERATED);
		}
		if (this->renderer != nullptr) {
			this->font = TTF_OpenFont("resources/OpenSans-Semibold.ttf", 24);
			this->music = Mix_LoadMUS("resources/bit-shift.ogg");

			this->gopherTexture = IMG_LoadTexture(this->renderer, "resources/gopher-128px.png");
			this->gopherHitTexture = IMG_LoadTexture(this->renderer, "resources/gopher-hit-128px.png");
			this->attributionTexture = IMG_LoadTexture(this->renderer, "resources/attribution-text-block.png");

			this->gopherAppearSfx = Mix_LoadWAV("resources/gopher-appears.ogg");
			this->gopherHitSfx = Mix_LoadWAV("resources/gopher-hit.ogg");
			this->gopherDisappearSfx = Mix_LoadWAV("resources/gopher-disappears.ogg");
		}

		gopherPosition.x = 0;
		gopherPosition.y = 0;
	}

	~Impl() {
		if (this->font != nullptr) {
			TTF_CloseFont(this->font);
			this->font = nullptr;
		}
		if (this->music != nullptr) {
			Mix_FreeMusic(this->music);
		}

		if (this->gopherTexture != nullptr) {
			SDL_DestroyTexture(this->gopherTexture);
			this->gopherTexture = nullptr;
		}
		if (this->gopherHitTexture != nullptr) {
			SDL_DestroyTexture(this->gopherHitTexture);
			this->gopherHitTexture = nullptr;
		}
		if (this->attributionTexture != nullptr) {
			SDL_DestroyTexture(this->attributionTexture);
			this->attributionTexture = nullptr;
		}

		if (this->gopherAppearSfx != nullptr) {
			Mix_FreeChunk(this->gopherDisappearSfx);
			this->gopherAppearSfx = nullptr;
		}
		if (this->gopherHitSfx != nullptr) {
			Mix_FreeChunk(this->gopherHitSfx);
			this->gopherHitSfx = nullptr;
		}
		if (this->gopherDisappearSfx != nullptr) {
			Mix_FreeChunk(this->gopherDisappearSfx);
			this->gopherDisappearSfx = nullptr;
		}

		if (this->renderer != nullptr) {
			SDL_DestroyRenderer(this->renderer);
			this->renderer = nullptr;
		}
		if (this->window != nullptr) {
			SDL_DestroyWindow(this->window);
			this->window = nullptr;
		}
	}

	SDL_Window* createWindow() {
#ifdef __EMSCRIPTEN__
		int windowWidth = 960;
		int windowHeight = 540;
		int windowFlags = 0;
#else
		int windowWidth = 960;
		int windowHeight = 540;
		int windowFlags = SDL_WINDOW_RESIZABLE;
#endif

		SDL_Window* result = SDL_CreateWindow("Hit the Gopher!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, windowFlags);
		return result;
	}

	void renderCenteredText(const std::string& text, int y) const {
		SDL_Surface* surface = TTF_RenderUTF8_Blended(this->font, text.c_str(), SDL_Color{ 0x10, 0x10, 0x10, 0xFF });
		if (surface == nullptr) {
			throw std::runtime_error("TTF_RenderUTF8_Blended failed");
		}

		SDL_Texture* texture = SDL_CreateTextureFromSurface(this->renderer, surface);
		if (texture == nullptr) {
			throw std::runtime_error("SDL_CreateTextureFromSurface failed");
		}

		SDL_FreeSurface(surface);

		int windowWidth = 0, windowHeight = 0;
		SDL_GetWindowSize(this->window, &windowWidth, &windowHeight);

		int textWidth = 0, textHeight = 0;
		SDL_QueryTexture(texture, nullptr, nullptr, &textWidth, &textHeight);

		int textY = y;
		if (y == SDL_WINDOWPOS_CENTERED) {
			textY = (windowHeight / 2) - (textHeight / 2);
		}

		SDL_Rect textRect = { (windowWidth / 2) - (textWidth / 2), textY, textWidth, textHeight	};
		SDL_RenderCopy(this->renderer, texture, nullptr, &textRect);

		SDL_DestroyTexture(texture);
	}

	void renderAttribution() const {
		int windowWidth = 0, windowHeight = 0;
		SDL_GetWindowSize(this->window, &windowWidth, &windowHeight);

		int textWidth = 0, textHeight = 0;
		SDL_QueryTexture(this->attributionTexture, nullptr, nullptr, &textWidth, &textHeight);

		SDL_Rect textRect = { (windowWidth / 2) - (textWidth / 2), windowHeight - textHeight, textWidth, textHeight };
		SDL_RenderCopy(this->renderer, this->attributionTexture, nullptr, &textRect);
	}

	void transitionToPlayGame() {
		this->gameState = GopherGameState::PLAY_GAME;

		int musicVolume = static_cast<int>(round(0.25f * MIX_MAX_VOLUME));
		Mix_VolumeMusic(musicVolume);
		Mix_PlayMusic(this->music, -1);
	}

	double scheduleGopherAppearance() {
		std::uniform_real_distribution<double> range(1.5, 3.0);
		double result = range(this->prng);
		return result;
	}

	double scheduleGopherDisappearance() {
		std::uniform_real_distribution<double> range(1.0, 1.5);
		double result = range(this->prng);
		return result;
	}

	SDL_Point decideGopherPosition() { // Cannot be const, because this->prng changes its state when it's called
		int windowWidth = 0, windowHeight = 0;
		SDL_GetWindowSize(this->window, &windowWidth, &windowHeight);

		std::uniform_int_distribution<int> xRange(10, windowWidth - 138);
		std::uniform_int_distribution<int> yRange(50, windowHeight - 138);

		SDL_Point result{ xRange(this->prng), yRange(this->prng)};
		return result;
	}

	void updateGopherAppearanceState(double secondsSinceLastUpdate) {
		if (this->gopherAppearanceState == GopherAppearanceState::WAITING) {
			this->secondsUntilAppearance = this->scheduleGopherAppearance();
			this->gopherAppearanceState = GopherAppearanceState::SCHEDULED;
		}
		else if (this->gopherAppearanceState == GopherAppearanceState::SCHEDULED) {
			this->secondsUntilAppearance -= secondsSinceLastUpdate;
			if (this->secondsUntilAppearance < 0.0) {
				Mix_PlayChannel(-1, this->gopherAppearSfx, 0);

				this->gopherAppearanceState = GopherAppearanceState::APPEARED;
				this->gopherPosition = decideGopherPosition();
				this->secondsUntilDisappearance = this->scheduleGopherDisappearance();
			}
		}
		else if ((this->gopherAppearanceState == GopherAppearanceState::APPEARED) || (this->gopherAppearanceState == GopherAppearanceState::HIT)) {
			this->secondsUntilDisappearance -= secondsSinceLastUpdate;
			if (this->secondsUntilDisappearance < 0.0) {
				if (this->gopherAppearanceState == GopherAppearanceState::APPEARED) {
					this->missCount++;
					Mix_PlayChannel(-1, this->gopherDisappearSfx, 0);
				}

				this->gopherAppearanceState = GopherAppearanceState::WAITING;
			}
		}
	}

	void attemptGopherHit(SDL_Point& clickPoint) {
		SDL_Rect gopherRect{ this->gopherPosition.x, this->gopherPosition.y, 128, 128 };
		if (SDL_PointInRect(&clickPoint, &gopherRect)) {
			this->gopherAppearanceState = GopherAppearanceState::HIT;
			this->hitCount++;

			Mix_PlayChannel(-1, this->gopherHitSfx, 0);
		}
		else {
			this->gopherAppearanceState = GopherAppearanceState::WAITING;
			this->missCount++;

			Mix_PlayChannel(-1, this->gopherDisappearSfx, 0);
		}
	}

};

GopherGame::GopherGame() : ptr(std::make_unique<Impl>()) {
}

GopherGame::~GopherGame() {
}

bool GopherGame::isInitialized() const {
	bool result =
		(ptr->window != nullptr) &&
		(ptr->renderer != nullptr) &&
		(ptr->font != nullptr) &&
		(ptr->music != nullptr) &&
		(ptr->gopherTexture != nullptr) &&
		(ptr->gopherHitTexture != nullptr) &&
		(ptr->gopherAppearSfx != nullptr) &&
		(ptr->gopherHitSfx != nullptr) &&
		(ptr->gopherDisappearSfx != nullptr);
	return result;
}

void GopherGame::handleEvent(const SDL_Event& event) {
	if (event.type == SDL_MOUSEBUTTONDOWN) {
		if (ptr->gameState == GopherGameState::WAIT_TO_START) {
			ptr->transitionToPlayGame();
		}
		else if (ptr->gameState == GopherGameState::PLAY_GAME) {
			if (
				(event.button.button == SDL_BUTTON_LEFT) &&
				(ptr->gopherAppearanceState == GopherAppearanceState::APPEARED)
			) {
				SDL_Point clickPoint{ event.button.x, event.button.y };
				ptr->attemptGopherHit(clickPoint);
			}
		}
	}
	else if (event.type == SDL_FINGERDOWN) {
		if (ptr->gameState == GopherGameState::WAIT_TO_START) {
			ptr->transitionToPlayGame();
		}
		else if (ptr->gameState == GopherGameState::PLAY_GAME) {
			if (ptr->gopherAppearanceState == GopherAppearanceState::APPEARED) {
				int windowWidth = 0, windowHeight = 0;
				SDL_GetWindowSize(ptr->window, &windowWidth, &windowHeight);

				SDL_Point clickPoint{ 0, 0 };
				clickPoint.x = static_cast<int>(round(event.tfinger.x * windowWidth));
				clickPoint.y = static_cast<int>(round(event.tfinger.y * windowHeight));
				ptr->attemptGopherHit(clickPoint);
			}
		}
	}
}

void GopherGame::updateState(std::chrono::duration<double> timerDelta) {
	double secondsSinceLastUpdate = timerDelta.count();

	if (ptr->gameState == GopherGameState::PLAY_GAME) {
		ptr->updateGopherAppearanceState(secondsSinceLastUpdate);
	}
}

void GopherGame::render() {
	SDL_SetRenderDrawColor(ptr->renderer, 240, 240, 240, 255);
	SDL_RenderClear(ptr->renderer);

	if (ptr->gameState == GopherGameState::WAIT_TO_START) {
		ptr->renderCenteredText("Click to Start", SDL_WINDOWPOS_CENTERED);
		ptr->renderAttribution();
	}
	else if (ptr->gameState == GopherGameState::PLAY_GAME) {
		std::string scoreString = "Hits: " + std::to_string(ptr->hitCount) + "     Misses: " + std::to_string(ptr->missCount);
		ptr->renderCenteredText(scoreString, 16);

		if (ptr->gopherAppearanceState == GopherAppearanceState::APPEARED) {
			SDL_Rect gopherRect{ ptr->gopherPosition.x, ptr->gopherPosition.y, 128, 128 };
			SDL_RenderCopy(ptr->renderer, ptr->gopherTexture, nullptr, &gopherRect);
		}
		else if (ptr->gopherAppearanceState == GopherAppearanceState::HIT) {
			SDL_Rect gopherRect{ ptr->gopherPosition.x, ptr->gopherPosition.y, 128, 128 };
			SDL_RenderCopy(ptr->renderer, ptr->gopherHitTexture, nullptr, &gopherRect);
		}
	}

	SDL_RenderPresent(ptr->renderer);
}
