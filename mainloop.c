#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

#include "mainloop.h"
#include "gameloop.h"
#include "ui.h"
#include "error.h"

extern char *kb_menu[];
extern char *skin_dir;

void mainloop(SDL_Window *win, SDL_Renderer *ren, int argc, char **argv) {

	int quit = 0;
	SDL_Event event;

	while(!quit) {
		while(SDL_PollEvent(&event)) {
			if(event.type == SDL_KEYDOWN) {
				switch(event.key.keysym.scancode) {
				case SDL_SCANCODE_Q:
					quit = 1;
					break;
				case SDL_SCANCODE_4:
				case SDL_SCANCODE_5:
				case SDL_SCANCODE_6:
				case SDL_SCANCODE_7:
				case SDL_SCANCODE_8:
				case SDL_SCANCODE_9:
					gameloop(win, ren, event.key.keysym.sym - 48, argv);
					break;
				}
			}
		}
	}
}


int render_menu(SDL_Window *win, SDL_Renderer *ren) {

}
