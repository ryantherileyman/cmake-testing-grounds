
#include <string>

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#endif

#include <SDL.h>
#include <SDL_ttf.h>

struct InitSdlResult {
    int sdlInitResult = -1;
    int sdlTtfInitResult = -1;
};

static InitSdlResult initializeSdl() {
    InitSdlResult result;
    result.sdlInitResult = SDL_Init(SDL_INIT_VIDEO);
    result.sdlTtfInitResult = TTF_Init();
    return result;
}

static bool sdlInitialized(const InitSdlResult& initResult) {
    bool result = (initResult.sdlInitResult == 0);
    result &= (initResult.sdlTtfInitResult == 0);
    return result;
}

static void shutdownSdl() {
    TTF_Quit();
    SDL_Quit();
}

#ifdef __EMSCRIPTEN__
EM_JS(void, enter_soft_fullscreen, (), {
    document.body.classList.add("soft-fullscreen");
});
EM_JS(void, exit_soft_fullscreen, (), {
    document.body.classList.remove("soft-fullscreen");
});
#endif

enum class RenderAlignment {
    ALIGN_START,
    ALIGN_CENTER,
    ALIGN_END,
};

struct SampleGame {
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    TTF_Font* font = nullptr;

    bool mouseWasClicked = false;
    int mouseX = 0;
    int mouseY = 0;
    bool windowWasResized = false;
    int windowSizeX = 0;
    int windowSizeY = 0;
    bool isFullscreen = false;

    SDL_Window* createWindow() {
#ifdef __EMSCRIPTEN__
        int windowWidth = 0, windowHeight = 0, windowFlags = 0;
        emscripten_get_canvas_element_size("#canvas", &windowWidth, &windowHeight);
#else
        int windowWidth = 960;
        int windowHeight = 540;
        int windowFlags = SDL_WINDOW_RESIZABLE;
#endif

        SDL_Window* result = SDL_CreateWindow("Testing Window Resize / Fullscreen / Orientation", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, windowFlags);
        return result;
    }

    SampleGame() {
        this->window = this->createWindow();
        if (this->window != nullptr) {
            Uint32 renderFlags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
            this->renderer = SDL_CreateRenderer(this->window, -1, renderFlags);
        }
        if (this->renderer != nullptr) {
            this->font = TTF_OpenFont("resources/OpenSans-Semibold.ttf", 16);
        }
    }

    ~SampleGame() {
        if (this->font != nullptr) {
            TTF_CloseFont(this->font);
        }
        if (this->renderer != nullptr) {
            SDL_DestroyRenderer(this->renderer);
        }
        if (this->window != nullptr) {
            SDL_DestroyWindow(this->window);
        }
    }

    bool isInitialized() const {
        bool result = (this->window != nullptr);
        result &= (this->renderer != nullptr);
        result &= (this->font != nullptr);
        return result;
    }

    SDL_Point getWindowSize() {
        SDL_Point result{ 0, 0 };
#ifdef __EMSCRIPTEN__
        emscripten_get_canvas_element_size("#canvas", &result.x, &result.y);
#else
        SDL_GetWindowSize(this->window, &result.x, &result.y);
#endif
        return result;
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

    SDL_Rect resolveFullscreenToggleRect() {
        SDL_Point windowSize = this->getWindowSize();

        SDL_Rect result{ windowSize.x / 2, 15, (windowSize.x / 2) - 15, 100 };
        return result;
    }

    void toggleFullscreen() {
        this->isFullscreen = !this->isFullscreen;
#ifdef __EMSCRIPTEN__
        if ( this->isFullscreen ) {
            enter_soft_fullscreen();
        } else {
            exit_soft_fullscreen();
        }
        this->resizeCanvas();
#else
        Uint32 flags = this->isFullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0;
        SDL_SetWindowFullscreen(this->window, flags);
#endif
    }

    void handleEvent(SDL_Event& event) {
        if (event.type == SDL_WINDOWEVENT) {
            if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                this->mouseWasClicked = false;
                this->windowWasResized = true;
                this->windowSizeX = event.window.data1;
                this->windowSizeY = event.window.data2;
            }
        }
        else if (event.type == SDL_MOUSEBUTTONDOWN) {
            this->windowWasResized = false;
            this->mouseWasClicked = false;

            SDL_Point clickPoint{ event.button.x, event.button.y };
            SDL_Rect fullscreenToggleRect = this->resolveFullscreenToggleRect();

            if (SDL_PointInRect(&clickPoint, &fullscreenToggleRect)) {
                this->toggleFullscreen();
            }
            else {
                this->mouseWasClicked = true;
                this->mouseX = event.button.x;
                this->mouseY = event.button.y;
            }
        }
    }

    void renderGrid() {
        SDL_Point windowSize = this->getWindowSize();

        SDL_SetRenderDrawColor(this->renderer, 0xCC, 0xCC, 0xCC, 0xFF);

        for ( int x = 100; x < windowSize.x; x += 100 ) {
            SDL_RenderDrawLine(this->renderer, x, 0, x, windowSize.y);
        }

        for ( int y = 100; y < windowSize.y; y += 100 ) {
            SDL_RenderDrawLine(this->renderer, 0, y, windowSize.x, y);
        }
    }

    void renderCornerRects() {
        SDL_Point windowSize = this->getWindowSize();

        SDL_SetRenderDrawColor(this->renderer, 0xFF, 0x33, 0x33, 0xFF);

        SDL_Rect cornerRectList[8] = {
            { 0, 0, 10, 3 },
            { 0, 0, 3, 10 },
            { 0, windowSize.y - 11, 3, 10 },
            { 0, windowSize.y - 4, 10, 3 },
            { windowSize.x - 11, 0, 10, 3 },
            { windowSize.x - 4, 0, 3, 10 },
            { windowSize.x - 11, windowSize.y - 4, 10, 3 },
            { windowSize.x - 4, windowSize.y - 11, 3, 10 },
        };

        SDL_RenderFillRects(this->renderer, cornerRectList, 8);
    }

    void renderFullscreenToggle() {
        SDL_Rect fullscreenRect = this->resolveFullscreenToggleRect();

        SDL_Point mousePos;
        SDL_GetMouseState(&mousePos.x, &mousePos.y);
        bool mouseIsOverToggle = SDL_PointInRect(&mousePos, &fullscreenRect);

        Uint8 toggleColor = mouseIsOverToggle ? 0xFF : 0xCC;
        SDL_SetRenderDrawColor(this->renderer, toggleColor, toggleColor, toggleColor, 0xFF);
        SDL_RenderFillRect(this->renderer, &fullscreenRect);
    }

    void renderAlignedText(const std::string& text, RenderAlignment horzAlign, RenderAlignment vertAlign, int padX, int padY, SDL_Color color) {
        SDL_Point windowSize = this->getWindowSize();

        SDL_Surface* surface = TTF_RenderUTF8_Blended(this->font, text.c_str(), color);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(this->renderer, surface);
        SDL_FreeSurface(surface);

        int textWidth = 0, textHeight = 0;
        SDL_QueryTexture(texture, nullptr, nullptr, &textWidth, &textHeight);

        int textX = 15 + padX;
        if ( horzAlign == RenderAlignment::ALIGN_CENTER ) {
            textX = (windowSize.x / 2) - (textWidth / 2);
        }
        else if ( horzAlign == RenderAlignment::ALIGN_END ) {
            textX = windowSize.x - textWidth - 15 - padX;
        }

        int textY = 15 + padY;
        if ( vertAlign == RenderAlignment::ALIGN_CENTER ) {
            textY = (windowSize.y / 2) - (textHeight / 2);
        }
        else if ( vertAlign == RenderAlignment::ALIGN_END ) {
            textY = windowSize.y - textHeight - 15 - padY;
        }

        SDL_Rect textRect = { textX, textY, textWidth, textHeight };
        SDL_RenderCopy(this->renderer, texture, nullptr, &textRect);

        SDL_DestroyTexture(texture);
    }

    void render() {
        SDL_SetRenderDrawColor(this->renderer, 240, 240, 240, 255);
        SDL_RenderClear(this->renderer);

        this->renderGrid();
        this->renderCornerRects();
        this->renderFullscreenToggle();

        std::string clickText = "Waiting for Mouse Button";
        if (this->windowWasResized) {
            clickText = "Resize: " + std::to_string(this->windowSizeX) + "," + std::to_string(this->windowSizeY);
        }
        else if (this->mouseWasClicked) {
            clickText = "Click: " + std::to_string(this->mouseX) + "," + std::to_string(this->mouseY);
        }
        this->renderAlignedText(clickText, RenderAlignment::ALIGN_START, RenderAlignment::ALIGN_CENTER, 0, 0, SDL_Color{ 0x10, 0x10, 0x10, 0xFF});
        this->renderAlignedText(clickText, RenderAlignment::ALIGN_CENTER, RenderAlignment::ALIGN_CENTER, 0, 0, SDL_Color{ 0x10, 0x10, 0x10, 0xFF});
        this->renderAlignedText(clickText, RenderAlignment::ALIGN_END, RenderAlignment::ALIGN_CENTER, 0, 0, SDL_Color{ 0x10, 0x10, 0x10, 0xFF});
        this->renderAlignedText(clickText, RenderAlignment::ALIGN_CENTER, RenderAlignment::ALIGN_START, 0, 0, SDL_Color{ 0x10, 0x10, 0x10, 0xFF});
        this->renderAlignedText(clickText, RenderAlignment::ALIGN_CENTER, RenderAlignment::ALIGN_END, 0, 0, SDL_Color{ 0x10, 0x10, 0x10, 0xFF});

        SDL_Point windowSize = this->getWindowSize();

        std::string windowSizeText = std::to_string(windowSize.x) + "," + std::to_string(windowSize.y);
        this->renderAlignedText(windowSizeText, RenderAlignment::ALIGN_START, RenderAlignment::ALIGN_START, 0, 50, SDL_Color{ 0x33, 0x33, 0xCC, 0xFF});
        this->renderAlignedText(windowSizeText, RenderAlignment::ALIGN_START, RenderAlignment::ALIGN_END, 0, 50, SDL_Color{ 0x33, 0x33, 0xCC, 0xFF});
        this->renderAlignedText(windowSizeText, RenderAlignment::ALIGN_END, RenderAlignment::ALIGN_START, 0, 50, SDL_Color{ 0x33, 0x33, 0xCC, 0xFF});
        this->renderAlignedText(windowSizeText, RenderAlignment::ALIGN_END, RenderAlignment::ALIGN_END, 0, 50, SDL_Color{ 0x33, 0x33, 0xCC, 0xFF});

        SDL_RenderPresent(this->renderer);
    }
};

struct GameUserData {
    bool gameIsRunning = true;
    SampleGame sampleGame;
};

static void gameLoop(void* userData) {
    SDL_Event event;
    GameUserData* gameUserData = static_cast<GameUserData*>(userData);

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            gameUserData->gameIsRunning = false;
        }
        else {
            gameUserData->sampleGame.handleEvent(event);
        }
    }

    if (gameUserData->gameIsRunning) {
        gameUserData->sampleGame.render();
    }
}

#ifdef __EMSCRIPTEN__
static bool onEmscriptenResize(int eventType, const EmscriptenUiEvent* uiEvent, void* userData) {
    GameUserData* gameUserData = static_cast<GameUserData*>(userData);
    gameUserData->sampleGame.resizeCanvas();
    return true;
}
#endif

static bool runGame() {
    GameUserData gameUserData;

    bool result = gameUserData.sampleGame.isInitialized();
    if (result) {
#ifdef __EMSCRIPTEN__
        emscripten_set_resize_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, &gameUserData, false, onEmscriptenResize);
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
