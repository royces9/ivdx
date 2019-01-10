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


struct note *parse_map(int keys, FILE *fp, int early, int frame_time) {
	int time_point_count = 1000;

	struct note *out = calloc(time_point_count, sizeof(*out));


	/* for(int i = 0, prev = 1000; i < 200; ++i) { */
	/* 	out[i].objects = calloc(keys, sizeof(*out->objects)); */
	/* 	out[i].objects[0] = 2; */

	/* 	out[i].times.start = prev + frame_time; */
	/* 	prev += frame_time; */
	/* } */


	for(int i = 0, prev = 5000; i < 1000; ++i) {
		out[i].objects = calloc(keys, sizeof(*out->objects));
		out[i].objects[i % 4] = 1;

		out[i].times.start = prev + 250;
		prev += 250;
	}

	return out;
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

	SDL_Texture **note_tex = malloc(keys * sizeof(*note_tex));
	SDL_Texture **hold_tex = malloc(keys * sizeof(*hold_tex));

	SDL_Rect **note_rect = malloc(keys * sizeof(*note_rect));
	SDL_Rect **hold_rect = malloc(keys * sizeof(*hold_rect));

	unsigned char *head = calloc(keys, sizeof(*head));
	unsigned char *tail = calloc(keys, sizeof(*tail));

	unsigned char *hold_head = calloc(keys, sizeof(*hold_head));
	unsigned char *hold_tail = calloc(keys, sizeof(*hold_tail));

	int *const scancode = kb_game[keys - 4];

	for(int i = 0; i < keys; ++i) {
		*(note_tex + i) = IMG_LoadTexture(win->r, "pink.jpg");
		*(hold_tex + i) = IMG_LoadTexture(win->r, "pink.jpg");
		if(!(*(note_tex + i)) || !(*(hold_tex + i))) {
			SDL_err();
			return;
		}

		*(note_rect + i) = malloc(MAX_SIZE * sizeof(**note_rect));
		*(hold_rect + i) = malloc(MAX_SIZE * sizeof(**hold_rect));
		for(int j = 0; j < MAX_SIZE; ++j) {
			//change i to grab from skin instead
			if(load_rect(*(note_tex + i), *(note_rect + i) + j, -1, -1, i, 0)) {
				SDL_err();
				return;
			}

			if(load_rect(*(hold_tex + i), *(hold_rect + i) + j, -1, -1, i, 0)) {
				SDL_err();
				return;
			}
		}
	}

	FILE *fp;
	struct note *notes = parse_map(keys, fp, draw_early, ms_per_frame);

	if(SDL_RenderClear(win->r)) {
		SDL_err();
		goto return_;
	}	


	long lag = 0;
	int hold_ind = 0;
	int object_ind = 0;

	int default_height = note_rect[0][0].h;

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
			int diff = offset - notes[object_ind].times.start;

			if(diff < 0)
				diff *= -1;

			if(diff < ms_per_frame) {
				object_ind++;
				for(int i = 0; i < keys; ++i) {
					if(notes[object_ind].objects[i] == 1) {
						note_rect[i][head[i]++].h = default_height;
					} else if(notes[object_ind].objects[i] == 2) {
						note_rect[i][head[i]++].h =
							(notes[object_ind].times.end -
							 notes[object_ind].times.start) /
							ms_per_frame;
					}
					/*
					if(notes[object_ind].objects[i] == 1) {
						head[i]++;
					} else if(notes[object_ind].objects[i] == 2) {
						
						hold_rect[i][hold_head[i]++].h = (notes[object_ind].times.end - notes[object_ind].times.start)/ms_per_frame;
						hold_head[i]++;
					}
					*/
				}
			}

			if(update_note(win, keys, MAX_SIZE, note_rect, delta_pos, head, tail))
				goto return_;
		}

		for(int i = 0; i < keys; ++i) {
			for(unsigned char j = tail[i]; j != head[i]; ++j) {
				if(SDL_RenderCopy(win->r, *(note_tex + i), NULL, &note_rect[i][j])) {
					SDL_err();
					goto return_;
				}

				/*
				if(SDL_RenderCopy(win->r, *(hold_tex + i), NULL, &hold_rect[i][j])) {
					SDL_err();
					goto return_;
				}
				*/
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

	//free the note struct

	for(int i = 0; i < keys; ++i)
		SDL_DestroyTexture(*(note_tex + i));

	free(note_tex);

	for(int i = 0; i < keys; ++i)
		free(*(note_rect + i));

	free(note_rect);
}


int update_note(win_ren *win, int key_count, int note_count, SDL_Rect **note, int increment, unsigned char *head, unsigned char *tail) {
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


int load_rect(SDL_Texture *tex, SDL_Rect *rect, int width, int height, int lane_no, int y) {
	if(SDL_QueryTexture(tex, NULL, NULL, &rect->w, &rect->h)) {
		SDL_err();
		return 1;
	}

	rect->x = (lane_no + 1) * rect->w;
	rect->y = y;

	return 0;
}
