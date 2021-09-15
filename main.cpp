#include <SDL.h>
#include <SDL_image.h>

#include <iostream>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

#define COLOR_BLACK 0xFF000000
#define COLOR_WHITE 0xFFFFFFFF
#define COLOR_RED   0xFFFF0000
#define COLOR_GREEN 0xFF00FF00
#define COLOR_BLUE  0xFF0000FF

void Render(SDL_Window *window, SDL_Surface *screen, SDL_Surface *img) {
	SDL_Rect rect;
	SDL_FillRect(screen, NULL, COLOR_BLACK);
	rect.w = SCREEN_WIDTH;
	rect.h = SCREEN_HEIGHT;
	rect.x = 0;
	rect.y = 0;
	SDL_FillRect(screen, &rect, COLOR_WHITE);

	rect.w = img->w;
	rect.h = img->h;
	SDL_BlitSurface(img, &rect, screen, NULL);
	SDL_UpdateWindowSurface(window);
}

void pencil(SDL_Surface *img, int x, int y) {
	/* warning: endianness is ABGR */
	uint32_t* pixels = (uint32_t*)img->pixels;
	pixels[(y*img->w)+x] = COLOR_BLACK;
}

int main(int argc, char* args[]) {
	// initialize SDL
	SDL_Init(SDL_INIT_VIDEO);
	IMG_Init(IMG_INIT_PNG);

	// create window and screen
	SDL_Window* window = SDL_CreateWindow("PaintAudio", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	SDL_Surface* screen = SDL_GetWindowSurface(window);

	//// load optimized image
	//SDL_Surface* loadedSurface = IMG_Load("../loaded.png");
	//SDL_Surface* image = SDL_ConvertSurface(loadedSurface, loadedSurface->format, 0 );
	//SDL_FreeSurface(loadedSurface);

	SDL_Surface* canvas = SDL_CreateRGBSurfaceWithFormat(NULL, SCREEN_WIDTH, SCREEN_HEIGHT, 8, SDL_PIXELFORMAT_BGRA32);
	SDL_FillRect(canvas, NULL, COLOR_WHITE);
	Render(window, screen, canvas);

	bool quit = false, mouse_down = false;

	SDL_Event e;
	while (!quit) {
		bool should_draw = false;
		// handle event queue
		while (SDL_PollEvent(&e) != 0) {
			if (e.type == SDL_QUIT) { quit = true; }
			else if (e.type == SDL_KEYDOWN) {
				switch (e.key.keysym.sym) {
					case SDLK_ESCAPE:
						quit = true;
						break;
					}
			} else if (e.type == SDL_MOUSEBUTTONDOWN) {
				if (e.button.button == SDL_BUTTON_LEFT) {
					pencil(canvas, e.button.x, e.button.y);
					mouse_down = 1;
					should_draw = 1;
				}
			} else if (e.type == SDL_MOUSEBUTTONUP) {
				mouse_down = 0;
				should_draw = 1;
			} else if (e.type == SDL_MOUSEMOTION) {
				if (mouse_down && e.button.button == SDL_BUTTON_LEFT) {
					pencil(canvas, e.button.x, e.button.y);
					should_draw = 1;
				}
			}
		}

		if (should_draw) {
			Render(window, screen, canvas);
		}
	}

	SDL_FreeSurface(canvas);
	SDL_FreeSurface(screen);
	SDL_DestroyWindow(window);

	IMG_Quit();
	SDL_Quit();

	return 0;
}


