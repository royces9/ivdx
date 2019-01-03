#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include "init.h"
#include "error.h"

extern char *window_settings[];

void startup(void) {
        if( SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) ) {
		SDL_err();
		exit(1);
	}

	if(Mix_Init(MIX_INIT_FLAC | MIX_INIT_MOD | MIX_INIT_MP3 | MIX_INIT_OGG) == -1){
		printf("Mix_Init: %s\n", Mix_GetError());
		exit(1);
	}

	if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096) == -1){
		printf("Mix_OpenAudio: %s\n", Mix_GetError());
		exit(1);
	}
}

void init_window(struct window *window) {
	window->width = atoi(window_settings[0]);
	window->height = atoi(window_settings[1]);
	window->flags = SDL_WINDOW_SHOWN;
}

void init_render(struct render *render) {
	render->flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
}


void cleanup(SDL_Window *win, SDL_Renderer *ren) {
	if(win)
		SDL_DestroyWindow(win);
	
	if(ren)
		SDL_DestroyRenderer(ren);
}
