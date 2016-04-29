#include <SDL2/SDL.h>
#include <stdint.h>
#include "macros.h"

static SDL_Window *window;
static SDL_Renderer *renderer;

FUNCTION(sdl_Init) {
	VARS();

	SDL_Init(SDL_INIT_EVERYTHING);

	push(0);
}

FUNCTION(sdl_CreateWindow) {
	VARS();

	uint32_t height = pop();
	uint32_t width = pop();

	window = SDL_CreateWindow("koi sdl test",
			SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			width, height,
		   	0);
	if(!window) {
		push(-1);
		return;
	}

	renderer = SDL_CreateRenderer(window, -1, 0);
	if(!renderer) {
		push(-2);
		return;
	}

	push(0);
}

FUNCTION(sdl_Delay) {
	VARS();

	int32_t val = pop();
	SDL_Delay(val);

	push(0);
}

FUNCTION(sdl_Clear) {
	VARS();

	SDL_RenderClear(renderer);

	push(0);
}

FUNCTION(sdl_SetColor) {
	VARS();

	uint32_t b = pop();
	uint32_t g = pop();
	uint32_t r = pop();
	SDL_SetRenderDrawColor(renderer, r, g, b, 255);

	push(0);
}

FUNCTION(sdl_Present) {
	VARS();

	SDL_RenderPresent(renderer);

	push(0);
}

FUNCTION(sdl_HandleUpdates) {
	VARS();

	int32_t ret = 0;

	SDL_Event e;
	while(SDL_PollEvent(&e)) {
		switch(e.type) {
			case SDL_QUIT: 
				ret = -1;
				// TEMP
				SDL_Quit();
				break;
		}
	}

	push(ret);
}
