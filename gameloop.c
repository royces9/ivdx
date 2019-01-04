#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <stdlib.h>

#include "types.h"
#include "gameloop.h"
#include "error.h"

extern int *kb_game[];

#define MAX_SIZE 128

struct note {
	int occ;
	SDL_Rect rect;
};


void play_chart(win_ren *ren) {
}

void gameloop(win_ren *win, int argc, char **argv) {
        int quit = 0;
	int keys = argc;

	//scroll speed
	//units of (pixel / ms) * frame
	int speed = 20;

	//arbitrary scale amount, makes
	//speed a reasonable magnitude
	int scale = 10;

	//time per frame
	//ms/frame
	int ms_per_frame = 1000 / win->fr;

	//the number of pixels to move down per frame (each frame in time)
	//((pixel / ms) * frame) * (ms / frame) / (scale ms)
	int delta_pos = speed * ms_per_frame / scale;

	//ms to offset when the note should be
	//drawn w.r.t. when it should be hit
	//i.e. if timing is at 25ms, it should be
	//drawn earlier

	//hit_line is a placeholder for now
	int hit_line = 1000;
	int draw_early = hit_line * (scale / speed);

	SDL_Event event;
	SDL_Texture **tex = malloc(keys * sizeof(*tex));
	struct note **rect = malloc(keys * sizeof(*rect));

	int *note_ind = calloc(keys, sizeof(*note_ind));

	int *scancode = kb_game[keys - 4];

	for(int i = 0; i < keys; ++i) {
		*(tex + i) = IMG_LoadTexture(win->r, "pink.jpg");
		if(!(*(tex + i))) {
			SDL_err();
			return;
		}

		*(rect + i) = malloc(MAX_SIZE * sizeof(**rect));
		for(int j = 0; j < MAX_SIZE; ++j) {
			rect[i][j].rect.w = 100;
			rect[i][j].rect.h = 10;
			rect[i][j].rect.x = (i + 1) * 100;
			rect[i][j].rect.y = 0;
			rect[i][j].occ = 0;
		}
	}



	unsigned char const *state = SDL_GetKeyboardState(NULL);

	while(!quit) {
		unsigned int start = SDL_GetTicks();
		if(SDL_RenderClear(win->r)) {
			SDL_err();
			return;
		}

		while(SDL_PollEvent(&event)) {
			if(event.type == SDL_KEYDOWN) {
				if(event.key.keysym.scancode == SDL_SCANCODE_Q)
					return;
				
				int ind = 0;
				for(int i = 0; i < keys; ++i) {
					if(event.key.keysym.scancode == scancode[i]) {
						ind = i;
						break;
					}
				}
				rect[ind][note_ind[ind]++].occ = 1;
				if(note_ind[ind] >= MAX_SIZE)
					note_ind[ind] = 0;
			}
		}

		for(int i = 0; i < keys; ++i) {
			for(int j = 0; j < MAX_SIZE; ++j) {
				if(rect[i][j].occ) {
					if(SDL_RenderCopy(win->r, *(tex + i), NULL, &rect[i][j].rect)) {
						SDL_err();
						return;
					}
					
					rect[i][j].rect.y += delta_pos;
					if(rect[i][j].rect.y > hit_line) {
						rect[i][j].rect.y = 0;
						rect[i][j].occ = 0;
					}
				}
			}
		}
		SDL_RenderPresent(win->r);

		unsigned int end = SDL_GetTicks() - start;
		if(ms_per_frame > end)
			SDL_Delay(ms_per_frame - end);
		
	}
	
	/*
	while(!quit) {
		while(SDL_PollEvent(&event));

		if(state[SDL_SCANCODE_Q])
			return;

		if(SDL_RenderClear(win->r)) {
			SDL_err();
			return;
		}

		for(int i = 0; i < keys; ++i) {
			if(state[scancode[i]]) {
				rect[i][node_ind[i]++].occ = 1;

				if(note_ind[i] >= MAX_SIZE)
					note_ind[i] = 0;
			}

			for(int j = 0; j < MAX_SIZE; ++j) {
				if(rect[i][j].occ) {
					if(SDL_RenderCopy(win->r, *(tex + i), NULL, &rect[i][j].rect)) {
						SDL_err();
						return;
					}
					rect[i][j].rect.y += speed;
					if(rect[i][j].rect.y > 1080) {
						rect[i][j].rect.y = 0;
						rect[i][j].occ = 0;
					}
				}
			}
		}
		SDL_RenderPresent(win->r);

		SDL_Delay(16);
	}
	*/
	
	free(scancode);
	free(note_ind);
	for(int i = 0; i < keys; ++i)
		SDL_DestroyTexture(*(tex + i));

	free(tex);

	for(int i = 0; i < MAX_SIZE; ++i)
		free(*(rect + i));

	free(rect);
}
