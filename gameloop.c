#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <stdlib.h>

#include "types.h"
#include "gameloop.h"
#include "error.h"

extern int *kb_game[];

#define MAX_SIZE 64

int hit_line = 1080;


void play_chart(win_ren *ren) {
}

void gameloop(win_ren *win, int argc, char **argv) {
        int quit = 0;
	int keys = argc;

	//scroll speed
	//units of (pixel / s) * frame
	int speed = 400;

	//arbitrary scale amount, makes
	//speed a reasonable magnitude
	//ms
	float scale = 20;

	//time per frame
	//ms/frame
	int ms_per_frame = 1000 / win->fr;

	//the number of pixels to move down per frame (each frame in time)
	//((pixel / ms) * frame) * (ms / frame) / (ms) = pixel / ms
	//int delta_pos = speed * (float) ms_per_frame / scale;

	//(pixel / s) * frame / (frame / s)
	int delta_pos = speed / win->fr;

	//ms to offset when the note should be
	//drawn w.r.t. when it should be hit
	//i.e. if timing is at 25ms, it should be
	//drawn earlier

	//hit_line is a temporary global for now
	//the y coordinate of bottom of lane
	int draw_early = hit_line * (scale / speed);

	SDL_Event event;
	SDL_Texture **tex = malloc(keys * sizeof(*tex));
	struct note **rect = malloc(keys * sizeof(*rect));

	int *note_ind = calloc(keys, sizeof(*note_ind));

	int *const scancode = kb_game[keys - 4];

	for(int i = 0; i < keys; ++i) {
		*(tex + i) = IMG_LoadTexture(win->r, "pink.jpg");
		if(!(*(tex + i))) {
			SDL_err();
			return;
		}

		*(rect + i) = malloc(MAX_SIZE * sizeof(**rect));
		for(int j = 0; j < MAX_SIZE; ++j) {
			int width;
			int height;
			if(SDL_QueryTexture(*(tex + i), NULL, NULL, &width, &height)) {
				SDL_err();
				return;
			}
			
			rect[i][j].rect.w = width;
			rect[i][j].rect.h = height;
			rect[i][j].rect.x = (i + 1) * width;
			rect[i][j].rect.y = 0;
			rect[i][j].occ = 0;
		}
	}

	if(SDL_RenderClear(win->r)) {
		SDL_err();
		goto return_;
	}	


	long lag = 0;
	int update = 500/ win->fr;

	unsigned int prev = SDL_GetTicks();

	while(!quit) {
		unsigned int start = SDL_GetTicks();
		unsigned int elap = start - prev;
		prev = start;
		lag += (long) elap;

		while(SDL_PollEvent(&event)) {
			if(event.type == SDL_KEYDOWN) {
				if(event.key.keysym.scancode == SDL_SCANCODE_Q)
					goto return_;
				
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

		while(lag >= update) {
			if(update_note(win, keys, MAX_SIZE, tex, rect, delta_pos))
				goto return_;
			lag -= update;
		}

		for(int i = 0; i < keys; ++i) {
			for(int j = 0; j < MAX_SIZE; ++j) {
				if(rect[i][j].occ) {
					//rect[i][j].rect.y += ((float) lag / update) * (delta_pos);
					if(SDL_RenderCopy(win->r, *(tex + i), NULL, &rect[i][j].rect)) {
						SDL_err();
						goto return_;
					}
				}
			}
		}
		SDL_RenderPresent(win->r);
		if(SDL_RenderClear(win->r)) {
			SDL_err();
			goto return_;
		} 
		//unsigned int end = SDL_GetTicks() - start;

		//printf("%d\n", end);

 		/* if(end > ms_per_frame) { */
		/* 	SDL_RenderPresent(win->r); */
		/* } */

		/* if(ms_per_frame > end) */
		/* 	SDL_Delay(ms_per_frame - end); */
		
	}
	
	/*
	unsigned char const *state = SDL_GetKeyboardState(NULL);
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

 return_:
	free(note_ind);
	for(int i = 0; i < keys; ++i)
		SDL_DestroyTexture(*(tex + i));

	free(tex);


	for(int i = 0; i < keys; ++i)
		free(*(rect + i));

	free(rect);
}


int update_note(win_ren *win, int key_count, int note_count, SDL_Texture **tex, struct note **note, int increment) {
	for(int i = 0; i < key_count; ++i) {
		for(int j = 0; j < note_count; ++j) {
			if(note[i][j].occ) {
				/* if(SDL_RenderCopy(win->r, *(tex + i), NULL, &note[i][j].rect)) { */
				/* 	SDL_err(); */
				/* 	return 1; */
				/* } */
					
				note[i][j].rect.y += increment;
				if(note[i][j].rect.y > hit_line) {
					note[i][j].rect.y = 0;
					note[i][j].occ = 0;
				}
			}
		}
	}

	return 0;
}
