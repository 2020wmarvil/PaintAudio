#include <iostream>
#include <vector>

#include <SDL.h>
#include <SDL_image.h>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

#define COLOR_BLACK 0xFF000000
#define COLOR_WHITE 0xFFFFFFFF
#define COLOR_RED   0xFFFF0000
#define COLOR_GREEN 0xFF00FF00
#define COLOR_BLUE  0xFF0000FF

struct Point { int x, y; };

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

float Lerp(float start, float end, float t) {
    return start + t * (end-start);
}

Point LerpPoint(const Point& p1, const Point& p2, float t) {
	return { (int)Lerp(p1.x, p2.x, t), (int)Lerp(p1.y, p2.y, t) };
}

std::vector<Point> Line(const Point& p1, const Point& p2) {
	std::vector<Point> points;

	int N = std::max(std::abs(p1.x - p2.x), std::abs(p1.y - p2.y));
    for (int step = 0; step <= N; step++) {
        float t = N == 0 ? 0.0 : (step + 0.0f) / N;
        points.push_back(LerpPoint(p1, p2, t));
    }

    return points;
}

void Pencil(SDL_Surface *img, const Point& p1, const Point& p2) {
	std::vector<Point> points = Line(p1, p2);

	/* warning: endianness is ABGR */
	uint32_t* pixels = (uint32_t*)img->pixels;

	for (int i = 0; i < points.size(); i++) {
		int x = points[i].x;
		int y = points[i].y;

		pixels[(y*img->w)+x] = COLOR_BLACK;
	}
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

	Point formerMousePos;

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
					mouse_down = 1;
					should_draw = 1;

					Point mousePos = { e.button.x, e.button.y };
					Pencil(canvas, mousePos, mousePos);
					formerMousePos = mousePos;
				}
			} else if (e.type == SDL_MOUSEBUTTONUP) {
				mouse_down = 0;
				should_draw = 1;
			} else if (e.type == SDL_MOUSEMOTION) {
				if (mouse_down && e.button.button == SDL_BUTTON_LEFT) {
					Point mousePos = { e.button.x, e.button.y };
					Pencil(canvas, formerMousePos, mousePos);
					formerMousePos = mousePos;

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


