
#include <SDL2/SDL.h>

int main(int argc, char* argv[]) {

	SDL_Init(SDL_INIT_VIDEO);

	SDL_Window* window = SDL_CreateWindow("SDL2 Works!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, 0);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	bool windowIsOpen = true;

	while (windowIsOpen) {
		SDL_Event event;

		if (SDL_WaitEvent(&event)) {
			if (event.type == SDL_QUIT) {
				windowIsOpen = false;
				break;
			}
		}

		if (windowIsOpen) {
			SDL_Rect blueBox{ 200, 150, 200, 200 };

			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
			SDL_RenderClear(renderer);

			SDL_SetRenderDrawColor(renderer, 16, 16, 240, 255);
			SDL_RenderFillRect(renderer, &blueBox);

			SDL_RenderPresent(renderer);
		}
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
