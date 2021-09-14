/*This source code copyrighted by Lazy Foo' Productions (2004-2020)
and may not be redistributed without written permission.*/

//Using SDL and standard IO
#include <SDL.h>
#include <stdio.h>

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

int main( int argc, char* args[] ) {
	// initialize SDL
	SDL_Init(SDL_INIT_VIDEO);

	// create pointers
	SDL_Window *window = SDL_CreateWindow("PaintAudio", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	SDL_Surface* screenSurface = SDL_GetWindowSurface(window);

	bool quit = false;
	SDL_Event e;
	while (!quit) {
		// handle event queue
		while ( SDL_PollEvent( &e ) != 0 ) {
		    if ( e.type == SDL_QUIT ) { quit = true; }
		    else if ( e.type == SDL_KEYDOWN ) {
		        switch (e.key.keysym.sym) {
		            case SDLK_ESCAPE:
						quit = true;
						break;
		        }
		    }
		}

		// content
		SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0xFF, 0xFF, 0xFF));
		SDL_UpdateWindowSurface(window);
	}

	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}