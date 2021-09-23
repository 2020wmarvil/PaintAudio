#include <iostream>
#include <vector>
#include <stdio.h>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

#define COLOR_BLACK 0xFF000000
#define COLOR_WHITE 0xFFFFFFFF
#define COLOR_RED   0xFFFF0000
#define COLOR_GREEN 0xFF00FF00
#define COLOR_BLUE  0xFF0000FF

struct Point { int x, y; };

int pencilSize = 1;

static const char *arrow[] = {
  /* width height num_colors chars_per_pixel */
  "    32    32        3            1",
  /* colors */
  "X c #000000",
  ". c #ffffff",
  "  c None",
  /* pixels */
  "XXX                             ",
  "X..XX                           ",
  "X..X.XX                         ",
  " XX....XX                       ",
  " X.......XX                     ",
  "  X........X                    ",
  "  X.......X.X                   ",
  "   X.....X...X                  ",
  "   X....X.....X                 ",
  "    X..X.X.....X                ",
  "    X.X...X.....X               ",
  "     X.....X.....X              ",
  "      X.....X.....X             ",
  "       X.....X.....X            ",
  "        X.....X.....X           ",
  "         X.....X.....X          ",
  "          X.....X.....X         ",
  "           X.....X.....X        ",
  "            X.....X.....X       ",
  "             X.....X.....X      ",
  "              X.....X.....X     ",
  "               X.....X...X.X    ",
  "                X.....X.X...X   ",
  "                 X.....X...X.X  ",
  "                  X...X...X...X ",
  "                   X.X...X.....X",
  "                    X...X.....X ",
  "                     X.X.....X  ",
  "                      X.....X   ",
  "                       X...X    ",
  "                        X.X     ",
  "                         X      ",
  "0,0"
};

static SDL_Cursor *init_system_cursor(const char *image[]) {
  int i, row, col;
  Uint8 data[4*32];
  Uint8 mask[4*32];
  int hot_x, hot_y;

  i = -1;
  for (row=0; row<32; ++row) {
    for (col=0; col<32; ++col) {
      if (col % 8) {
        data[i] <<= 1;
        mask[i] <<= 1;
      } else {
        ++i;
        data[i] = mask[i] = 0;
      }
      switch (image[4+row][col]) {
        case 'X':
          data[i] |= 0x01;
          mask[i] |= 0x01;
          break;
        case '.':
          mask[i] |= 0x01;
          break;
        case ' ':
          break;
      }
    }
  }
  sscanf_s(image[4+row], "%d,%d", &hot_x, &hot_y);
  return SDL_CreateCursor(data, mask, 32, 32, hot_x, hot_y);
}

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
		for (int r = -pencilSize/2; r < std::ceil(pencilSize/2.0); r++) {
			for (int c = -pencilSize/2; c < std::ceil(pencilSize/2.0); c++) {
				int x = points[i].x + r;
				int y = points[i].y + c;

				pixels[(y*img->w)+x] = COLOR_BLACK;
			}
		}
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

	SDL_Cursor* cursor = init_system_cursor(arrow);
	SDL_SetCursor(cursor);

	SDL_Surface* canvas = SDL_CreateRGBSurfaceWithFormat(NULL, SCREEN_WIDTH, SCREEN_HEIGHT, 8, SDL_PIXELFORMAT_BGRA32);
	SDL_FillRect(canvas, NULL, COLOR_WHITE);
	Render(window, screen, canvas);

	TTF_Font *gFont = NULL;
	SDL_Surface* textSurface = TTF_RenderText_Solid(gFont, "Bonk", { 0, 0 , 0 });
	//SDL_Texture *texture = SDL_CreateTextureFromSurface( renderer, textSurface );

	bool quit = false, mouse_down = false;

	Point formerMousePos;

    int lctrl = 0, rctrl = 0;
    int lshift = 0, rshift = 0;

	SDL_StartTextInput();
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
					case SDLK_EQUALS:
						if (lshift || rshift) std::cout << "LOG: Pencil Size: " << ++pencilSize << std::endl;
						break;
					case SDLK_MINUS:
						if (--pencilSize < 1) pencilSize = 1;
						else std::cout << "LOG: Pencil Size: " << pencilSize << std::endl;
						break;
					case SDLK_RCTRL:
						rctrl = 1;
						break;
					case SDLK_LCTRL:
						lctrl = 1;
						break;
					case SDLK_RSHIFT:
						rshift = 1;
						break;
					case SDLK_LSHIFT:
						lshift = 1;
						break;
				}
			} else if (e.type == SDL_KEYUP) {
				switch (e.key.keysym.sym) {
					case SDLK_RCTRL:
						rctrl = 0;
						break;
					case SDLK_LCTRL:
						lctrl = 0;
						break;
					case SDLK_RSHIFT:
						rshift = 0;
						break;
					case SDLK_LSHIFT:
						lshift = 0;
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


