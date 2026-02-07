
#include <stdexcept>
#include <memory>
#include <string>
#include <random>
#include <cmath>
#include <chrono>

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#endif

#include <SDL_stdinc.h>
#include <SDL_video.h>
#include <SDL_rect.h>
#include <SDL_render.h>
#include <SDL_surface.h>
#include <SDL_pixels.h>
#include <SDL_events.h>
#include <SDL_mouse.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <SDL_image.h>

#include "GopherGame.hpp"

#ifdef __EMSCRIPTEN__
EM_JS(void, enter_soft_fullscreen, (), {
	document.body.classList.add("soft-fullscreen");
});
EM_JS(void, exit_soft_fullscreen, (), {
	document.body.classList.remove("soft-fullscreen");
});
#endif

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

constexpr int GOPHER_TEXTURE_SIZE = 128;

struct GopherGame::Impl {

	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;

	TTF_Font* standardFont = nullptr;
	TTF_Font* smallFont = nullptr;

	Mix_Music* music = nullptr;

	SDL_Texture* gopherTexture = nullptr;
	SDL_Texture* gopherHitTexture = nullptr;

	SDL_Texture* attributionTexture = nullptr;
	SDL_Texture* smallAttributionTexture = nullptr;

	SDL_Texture* fullscreenIconTexture = nullptr;

	Mix_Chunk* gopherAppearSfx = nullptr;
	Mix_Chunk* gopherHitSfx = nullptr;
	Mix_Chunk* gopherDisappearSfx = nullptr;

	GopherGameState gameState = GopherGameState::WAIT_TO_START;
	
	bool isFullscreen = false;
	bool mouseRecoveredFromFullscreenToggle = true;

	std::mt19937 prng{ std::random_device{}() }; // Could be marked "mutable"
	int hitCount = 0;
	int missCount = 0;

	GopherAppearanceState gopherAppearanceState = GopherAppearanceState::WAITING;
	SDL_Point gopherPosition;
	double secondsUntilAppearance = 0.0f;
	double secondsUntilDisappearance = 0.0f;

	SDL_Point getWindowSize() {
		SDL_Point result{ 0, 0 };
#ifdef __EMSCRIPTEN__
		emscripten_get_canvas_element_size("#canvas", &result.x, &result.y);
#else
		SDL_GetWindowSize(this->window, &result.x, &result.y);
#endif
		return result;
	}

	SDL_Window* createWindow() {
#ifdef __EMSCRIPTEN__
		SDL_Point windowSize = this->getWindowSize();
		int windowFlags = 0;
		emscripten_get_canvas_element_size("#canvas", &windowSize.x, &windowSize.y);
#else
		SDL_Point windowSize = { 960, 540 };
		int windowFlags = SDL_WINDOW_RESIZABLE;
#endif

		SDL_Window* result = SDL_CreateWindow("Hit the Gopher!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowSize.x, windowSize.y, windowFlags);
		return result;
	}

	Impl() {
		this->window = this->createWindow();
		if (this->window != nullptr) {
			SDL_SetWindowMinimumSize(this->window, 250, 250);

			int windowWidth = 0;
			int windowHeight = 0;
			SDL_GetWindowSize(this->window, &windowWidth, &windowHeight);

			this->renderer = SDL_CreateRenderer(this->window, -1, SDL_RENDERER_ACCELERATED);
			SDL_RenderSetLogicalSize(this->renderer, windowWidth, windowHeight);
		}
		if (this->renderer != nullptr) {
			this->standardFont = TTF_OpenFont("resources/OpenSans-Semibold.ttf", 24);
			this->smallFont = TTF_OpenFont("resources/OpenSans-Semibold.ttf", 16);

			this->music = Mix_LoadMUS("resources/bit-shift.ogg");

			this->gopherTexture = IMG_LoadTexture(this->renderer, "resources/gopher-128px.png");
			this->gopherHitTexture = IMG_LoadTexture(this->renderer, "resources/gopher-hit-128px.png");

			this->attributionTexture = IMG_LoadTexture(this->renderer, "resources/attribution-text-block.png");
			this->smallAttributionTexture = IMG_LoadTexture(this->renderer, "resources/attribution-text-block-small.png");

			this->fullscreenIconTexture = IMG_LoadTexture(this->renderer, "resources/fullscreen-icons.png");

			this->gopherAppearSfx = Mix_LoadWAV("resources/gopher-appears.ogg");
			this->gopherHitSfx = Mix_LoadWAV("resources/gopher-hit.ogg");
			this->gopherDisappearSfx = Mix_LoadWAV("resources/gopher-disappears.ogg");
		}

		gopherPosition.x = 0;
		gopherPosition.y = 0;
	}

	~Impl() {
		if (this->standardFont != nullptr) {
			TTF_CloseFont(this->standardFont);
			this->standardFont = nullptr;
		}
		if (this->smallFont != nullptr) {
			TTF_CloseFont(this->smallFont);
			this->smallFont = nullptr;
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
		if (this->smallAttributionTexture != nullptr) {
			SDL_DestroyTexture(this->smallAttributionTexture);
			this->smallAttributionTexture = nullptr;
		}

		if (this->fullscreenIconTexture != nullptr) {
			SDL_DestroyTexture(this->fullscreenIconTexture);
			this->fullscreenIconTexture = nullptr;
		}

		if (this->gopherAppearSfx != nullptr) {
			Mix_FreeChunk(this->gopherAppearSfx);
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

#ifdef __EMSCRIPTEN__
	void resizeCanvas() {
		double canvasCssWidth = 0.0, canvasCssHeight = 0.0;
		emscripten_get_element_css_size("#canvas", &canvasCssWidth, &canvasCssHeight);
		double dpr = emscripten_get_device_pixel_ratio();

		int canvasWidth = (int)std::floor(canvasCssWidth * dpr);
		int canvasHeight = (int)std::floor(canvasCssHeight * dpr);

		emscripten_set_canvas_element_size("#canvas", canvasWidth, canvasHeight);
		SDL_SetWindowSize(this->window, canvasWidth, canvasHeight);
	}
#endif

	SDL_Point resolveLogicalWindowSize(int windowWidth, int windowHeight, int clampWidth, int clampHeight) const {
		double aspectRatio = (double)windowWidth / (double)windowHeight;

		SDL_Point result{ windowWidth, windowHeight };

		if (windowWidth > windowHeight) {
			result.x = (int)std::ceil((double)clampHeight * aspectRatio);
			result.y = clampHeight;
		}
		else {
			result.x = clampWidth;
			result.y = (int)std::ceil((double)clampWidth / aspectRatio);
		}

		return result;
	}

	SDL_Rect resolveFullscreenIconRect() const {
		int windowWidth = 0, windowHeight = 0;
		SDL_RenderGetLogicalSize(this->renderer, &windowWidth, &windowHeight);

		SDL_Rect result = { windowWidth - 48, 12, 32, 32 };
		return result;
	}

	void onWindowResize() {
		int windowWidth = 0, windowHeight = 0;
		SDL_GetWindowSize(this->window, &windowWidth, &windowHeight);

		SDL_Point logicalWindowSize{ windowWidth, windowHeight };

		// Only use a logical size that differs from window size for extremely small windows
		if ((windowWidth < 480) || (windowHeight < 270)) {
			logicalWindowSize = this->resolveLogicalWindowSize(windowWidth, windowHeight, 480, 270);
		}

		bool logicalSizeEqualsWindowSize = ((logicalWindowSize.x == windowWidth) && (logicalWindowSize.y == windowHeight));
		SDL_RenderSetLogicalSize(this->renderer, logicalWindowSize.x, logicalWindowSize.y);
		SDL_RenderSetIntegerScale(this->renderer, logicalSizeEqualsWindowSize ? SDL_TRUE : SDL_FALSE);

		// Web-based environments don't need this logic to warp the mouse back into the window, but it's harmless
		if ( !this->mouseRecoveredFromFullscreenToggle ) {
			SDL_Rect fullscreenIconRect = this->resolveFullscreenIconRect();
			SDL_WarpMouseInWindow(this->window, fullscreenIconRect.x, fullscreenIconRect.y);

			this->mouseRecoveredFromFullscreenToggle = true;
		}
	}

	void toggleFullscreen() {
		this->isFullscreen = !this->isFullscreen;
		this->mouseRecoveredFromFullscreenToggle = false;

#ifdef __EMSCRIPTEN__
		if ( this->isFullscreen ) {
			enter_soft_fullscreen();
		}
		else {
			exit_soft_fullscreen();
		}
		this->resizeCanvas();
#else
		Uint32 flags = this->isFullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0;
		SDL_SetWindowFullscreen(this->window, flags);
#endif
	}

	bool isSmallWindowSize(int windowWidth, int windowHeight) const {
		bool result = ((windowWidth < 640) || (windowHeight < 360));
		return result;
	}

	void renderCenteredText(const std::string& text, int y) const {
		int windowWidth = 0, windowHeight = 0;
		SDL_RenderGetLogicalSize(this->renderer, &windowWidth, &windowHeight);

		TTF_Font* uiFont = this->standardFont;
		if (this->isSmallWindowSize(windowWidth, windowHeight)) {
			uiFont = this->smallFont;
		}

		SDL_Surface* surface = TTF_RenderUTF8_Blended(uiFont, text.c_str(), SDL_Color{ 0x10, 0x10, 0x10, 0xFF });
		if (surface == nullptr) {
			throw std::runtime_error("TTF_RenderUTF8_Blended failed");
		}

		SDL_Texture* texture = SDL_CreateTextureFromSurface(this->renderer, surface);
		if (texture == nullptr) {
			throw std::runtime_error("SDL_CreateTextureFromSurface failed");
		}

		SDL_FreeSurface(surface);

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
		SDL_RenderGetLogicalSize(this->renderer, &windowWidth, &windowHeight);

		SDL_Texture* uiTexture = this->attributionTexture;
		if (this->isSmallWindowSize(windowWidth, windowHeight)) {
			uiTexture = this->smallAttributionTexture;
		}

		int textWidth = 0, textHeight = 0;
		SDL_QueryTexture(uiTexture, nullptr, nullptr, &textWidth, &textHeight);

		SDL_Rect textRect = { (windowWidth / 2) - (textWidth / 2), windowHeight - textHeight, textWidth, textHeight };
		SDL_RenderCopy(this->renderer, uiTexture, nullptr, &textRect);
	}

	void renderFullscreenIcon() const {
		SDL_Rect srcRect = { this->isFullscreen ? 32 : 0, 0, 32, 32 };
		SDL_Rect destRect = this->resolveFullscreenIconRect();

		SDL_Point mousePos;
		SDL_GetMouseState(&mousePos.x, &mousePos.y);
		bool mouseIsOverIcon = SDL_PointInRect(&mousePos, &destRect);

		Uint8 iconColorMod = mouseIsOverIcon ? 0xFF : 0xCC;
		SDL_SetTextureColorMod(this->fullscreenIconTexture, iconColorMod, iconColorMod, iconColorMod);

		SDL_RenderCopy(this->renderer, this->fullscreenIconTexture, &srcRect, &destRect);
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
		SDL_RenderGetLogicalSize(this->renderer, &windowWidth, &windowHeight);

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
		SDL_Rect gopherRect{ this->gopherPosition.x, this->gopherPosition.y, GOPHER_TEXTURE_SIZE, GOPHER_TEXTURE_SIZE };
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
		(ptr->smallFont != nullptr) &&
		(ptr->standardFont != nullptr) &&
		(ptr->music != nullptr) &&
		(ptr->gopherTexture != nullptr) &&
		(ptr->gopherHitTexture != nullptr) &&
		(ptr->attributionTexture != nullptr) &&
		(ptr->smallAttributionTexture != nullptr) &&
		(ptr->fullscreenIconTexture != nullptr) &&
		(ptr->gopherAppearSfx != nullptr) &&
		(ptr->gopherHitSfx != nullptr) &&
		(ptr->gopherDisappearSfx != nullptr);
	return result;
}

#ifdef __EMSCRIPTEN__
void GopherGame::resizeCanvas() {
	ptr->resizeCanvas();
}
#endif

void GopherGame::handleEvent(const SDL_Event& event) {
	if (event.type == SDL_WINDOWEVENT) {
		if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
			ptr->onWindowResize();
		}
	}
	// Note that SDL generates synthetic mouse events for touch events
	else if (event.type == SDL_MOUSEBUTTONDOWN) {
		SDL_Point clickPoint{ event.button.x, event.button.y };
		SDL_Rect fullscreenIconRect = ptr->resolveFullscreenIconRect();

		if ( SDL_PointInRect(&clickPoint, &fullscreenIconRect) ) {
			ptr->toggleFullscreen();
		}
		else if (ptr->gameState == GopherGameState::WAIT_TO_START) {
			ptr->transitionToPlayGame();
		}
		else if (ptr->gameState == GopherGameState::PLAY_GAME) {
			if (
				(event.button.button == SDL_BUTTON_LEFT) &&
				(ptr->gopherAppearanceState == GopherAppearanceState::APPEARED)
			) {
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
			SDL_Rect gopherRect{ ptr->gopherPosition.x, ptr->gopherPosition.y, GOPHER_TEXTURE_SIZE, GOPHER_TEXTURE_SIZE };
			SDL_RenderCopy(ptr->renderer, ptr->gopherTexture, nullptr, &gopherRect);
		}
		else if (ptr->gopherAppearanceState == GopherAppearanceState::HIT) {
			SDL_Rect gopherRect{ ptr->gopherPosition.x, ptr->gopherPosition.y, GOPHER_TEXTURE_SIZE, GOPHER_TEXTURE_SIZE };
			SDL_RenderCopy(ptr->renderer, ptr->gopherHitTexture, nullptr, &gopherRect);
		}
	}

	ptr->renderFullscreenIcon();

	SDL_RenderPresent(ptr->renderer);
}
