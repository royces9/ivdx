#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include "types.h"
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

void start_window(win_ren *data) {
	char *title = "ivdx";

	int width = atoi(window_settings[0]);
	int height = atoi(window_settings[1]);
	int framerate = atoi(window_settings[2]);
	int w_flags = SDL_WINDOW_SHOWN;

	SDL_Window *win = SDL_CreateWindow(title, 0, 0, width, height, w_flags);
	if(!win) {
		SDL_err();
		exit(1);
	}


	int r_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;

	SDL_Renderer *ren = SDL_CreateRenderer(win, -1, r_flags);
	if(!ren) {
		SDL_err();
		exit(1);
	}

	data->w = win;
	data->r = ren;
	data->fr = framerate;		    
}


void cleanup(win_ren *w) {
	if(w->w)
		SDL_DestroyWindow(w->w);
	
	if(w->r)
		SDL_DestroyRenderer(w->r);
}
