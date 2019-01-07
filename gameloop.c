#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <stdlib.h>

#include "types.h"
#include "gameloop.h"
#include "error.h"

extern int *kb_game[];

//#define MAX_SIZE 64
#define MAX_SIZE 256

int hit_line = 1080;



void play_chart(win_ren *ren) {
}

struct note *parse_map(int keys, FILE *fp, int early) {
}

void gameloop(win_ren *win, int argc, char **argv) {
        int quit = 0;
	int keys = argc;

	//scroll speed
	//units of (pixel / s)
	int speed = 1000;

	//time per frame
	//ms/frame
	int ms_per_frame = 1000 / win->fr;

	//the number of pixels to move down per frame (each frame in time)
	//(pixel / s) / (frame / s)
	int delta_pos = speed / win->fr;

	//ms to offset when the note should be
	//drawn w.r.t. when it should be hit
	//i.e. if timing is at 25ms, it should be
	//drawn earlier

	//hit_line is a temporary global for now
	//the y coordinate of bottom of lane
	int draw_early = (hit_line * 1000) / speed;

	SDL_Event event;
	SDL_Texture **tex = malloc(keys * sizeof(*tex));
	SDL_Rect **rect = malloc(keys * sizeof(*rect));

	unsigned char *head = calloc(keys, sizeof(*head));
	unsigned char *tail = calloc(keys, sizeof(*tail));

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
			
			rect[i][j].w = width;
			rect[i][j].h = height;
			rect[i][j].x = (i + 1) * width;
			rect[i][j].y = 0;
		}
	}

	FILE *fp;
	struct note *notes = parse_map(keys, fp, draw_early);

	if(SDL_RenderClear(win->r)) {
		SDL_err();
		goto return_;
	}	


	long lag = 0;

	int object_ind = 0;
	unsigned int prev = SDL_GetTicks();
	unsigned int start = prev;
	while(!quit) {
		while(SDL_PollEvent(&event)) {
			if(event.type == SDL_KEYDOWN) {
				if(event.key.keysym.scancode == SDL_SCANCODE_Q)
					goto return_;
				
				for(int i = 0; i < keys; ++i) {
					if(event.key.keysym.scancode == scancode[i]) {
						head[i]++;
						break;
					}
				}
			}
		}
		
		for(unsigned int curr; (curr = SDL_GetTicks()) > prev; prev += ms_per_frame) {
			unsigned int offset = curr - start;
			int diff = (int) offset - notes->times[object_ind];

			if(diff < 0)
				diff *= -1;

			if(diff < ms_per_frame) {
				object_ind++;
				for(int i = 0; i < keys; ++i) {
					if(notes->objects[i][object_ind] == 1) {
						head[i]++;
					}
				}
			}

			if(update_note(win, keys, MAX_SIZE, tex, rect, delta_pos, head, tail))
				goto return_;

		}

		for(int i = 0; i < keys; ++i) {
			for(unsigned char j = tail[i]; j != head[i]; ++j) {
				if(SDL_RenderCopy(win->r, *(tex + i), NULL, &rect[i][j])) {
					SDL_err();
					goto return_;
				}
			}
		}

		SDL_RenderPresent(win->r);

		if(SDL_RenderClear(win->r)) {
			SDL_err();
			goto return_;
		}
	}

 return_:
	free(head);
	free(tail);
	for(int i = 0; i < keys; ++i)
		SDL_DestroyTexture(*(tex + i));

	free(tex);


	for(int i = 0; i < keys; ++i)
		free(*(rect + i));

	free(rect);
}


int update_note(win_ren *win, int key_count, int note_count, SDL_Texture **tex, SDL_Rect **note, int increment, unsigned char *head, unsigned char *tail) {
	for(int i = 0; i < key_count; ++i) {
		for(unsigned char j = tail[i]; j != head[i]; ++j) {
			note[i][j].y += increment;

			if(note[i][j].y > hit_line) {
				tail[i]++;
				note[i][j].y = 0;
			}
		}
	}

	return 0;
}
