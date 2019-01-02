#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "ui.h"
#include "error.h"

void draw_static(SDL_Renderer *ren, char *file, int x, int y, int w, int h) {
	SDL_Texture *tex = IMG_LoadTexture(ren, file);
	if(!tex) {
		SDL_err();
		return;
	}

	SDL_Rect rect;
	rect.x = x;
	rect.y = y;
	rect.w = w;
	rect.h = h;
	SDL_RenderCopy(ren, tex, NULL, &rect);
}


void draw_move(SDL_Renderer *ren, SDL_Texture *tex, SDL_Rect rect, int x, int y, int w, int h, int endx, int endy, int t) {
	int diffx = endx - x;
	int diffy = endy - y;

	float chngx = (float)diffx / t;
	float chngy = (float)diffy / t;

	for(int i = 0; i < t; ++i) {
		rect.y = y + i * chngy;
		rect.x = x + i * chngx;
		SDL_RenderClear(ren);
		SDL_RenderCopy(ren, tex, NULL, &rect);
		SDL_RenderPresent(ren);
	}
}
