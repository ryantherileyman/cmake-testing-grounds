
#include <SDL3/SDL.h>

int main(int argc, char* argv[]) {

	SDL_Init(SDL_INIT_VIDEO);

	SDL_Window* window = SDL_CreateWindow("SDL3 Works!", 800, 600, 0);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);

	bool windowIsOpen = true;

	while (windowIsOpen) {
		SDL_Event event;

		if (SDL_WaitEvent(&event)) {
			if (event.type == SDL_EVENT_QUIT) {
				windowIsOpen = false;
			}
		}

		if (windowIsOpen) {
			SDL_FRect greenBox{ 200, 15, 200, 200 };

			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
			SDL_RenderClear(renderer);

			SDL_SetRenderDrawColor(renderer, 16, 240, 16, 255);
			SDL_RenderFillRect(renderer, &greenBox);

			SDL_RenderPresent(renderer);
		}
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
