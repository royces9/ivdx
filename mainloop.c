#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

#include "mainloop.h"
#include "gameloop.h"
#include "ui.h"
#include "error.h"

void mainloop(SDL_Window *win, SDL_Renderer *ren, int argc, char **argv) {

	int quit = 0;
	SDL_Event event;

	while(!quit) {
		while(SDL_PollEvent(&event)) {
			if(event.type == SDL_KEYDOWN) {
				switch(event.key.keysym.sym) {
				case SDLK_q:
					quit = 1;
					break;
				case SDLK_4:
				case SDLK_5:
				case SDLK_6:
				case SDLK_7:
				case SDLK_8:
				case SDLK_9:
					gameloop(win, ren, event.key.keysym.sym - 48, argv);
					break;
				}
			}
		}
	}
}
