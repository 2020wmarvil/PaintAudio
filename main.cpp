#include <SDL.h>
#include <SDL_image.h>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

int main(int argc, char* args[]) {
	// initialize SDL
	SDL_Init(SDL_INIT_VIDEO);
	IMG_Init(IMG_INIT_PNG);

	// create window and screen
	SDL_Window *window = SDL_CreateWindow("PaintAudio", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	SDL_Surface* screen = SDL_GetWindowSurface(window);

	// load optimized image
	SDL_Surface* loadedSurface = IMG_Load("../loaded.png");
	SDL_Surface* image = SDL_ConvertSurface(loadedSurface, loadedSurface->format, 0 );
	SDL_FreeSurface(loadedSurface);

	bool quit = false;
	SDL_Event e;
	while (!quit) {
		// handle event queue
		while (SDL_PollEvent(&e) != 0) {
		    if (e.type == SDL_QUIT) { quit = true; }
		    else if (e.type == SDL_KEYDOWN) {
		        switch (e.key.keysym.sym) {
		            case SDLK_ESCAPE:
						quit = true;
						break;
		        }
		    }
		}

		// content
		SDL_BlitSurface(image, NULL, screen, NULL);
		SDL_UpdateWindowSurface(window);
	}

	SDL_FreeSurface(image);
	SDL_FreeSurface(screen);
	SDL_DestroyWindow(window);

	IMG_Quit();
	SDL_Quit();

	return 0;
}

