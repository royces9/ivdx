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
	int object_count = 1000;

	struct note *out = calloc(object_count, sizeof(*out));

	out[0].times.start = 100;
	out[0].times.end = 0;
	out[0].objects = calloc(keys, sizeof(*out->objects));
	out[0].objects[0] = 1;

	out[1].times.start = 200;
	out[1].times.end = 0;
	out[1].objects = calloc(keys, sizeof(*out->objects));
	out[1].objects[1] = 1;
	out[1].objects[2] = 1;

	out[2].times.start = 300;
	out[2].times.end = 400;
	out[2].objects = calloc(keys, sizeof(*out->objects));
	out[2].objects[0] = 2;


	for(int i = 3, prev = 2000; i < object_count; ++i) {
		out[i].objects = calloc(keys, sizeof(*out->objects));
		out[i].objects[0] = 1;

		out[i].times.start = prev;
		prev += 50;
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
		note_tex[i] = IMG_LoadTexture(win->r, "pink.jpg");
		hold_tex[i] = IMG_LoadTexture(win->r, "pink.jpg");
		if(!(note_tex[i]) || !(hold_tex[i])) {
			SDL_err();
			return;
		}

		note_rect[i] = malloc(MAX_SIZE * sizeof(**note_rect));
		hold_rect[i] = malloc(MAX_SIZE * sizeof(**hold_rect));
		/*
		for(int j = 0; j < MAX_SIZE; ++j) {
			//change i to grab from skin instead
			if(load_rect(note_tex[i], note_rect[i] + j, -1, -1, (i + 1) * 100, 0)) {
				SDL_err();
				return;
			}

			if(load_rect(hold_tex[i], hold_rect[i] + j, -1, -1, i, 0)) {
				SDL_err();
				return;
			}
		}
		*/
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
				for(int i = 0; i < keys; ++i) {
					if(notes[object_ind].objects[i]) {
						if(notes[object_ind].objects[i] == 1) {
							note_rect[i][head[i]].h = default_height;
							note_rect[i][head[i]].y = -default_height;
						} else if(notes[object_ind].objects[i] == 2) {
							int time_diff =	notes[object_ind].times.end - notes[object_ind].times.start;
							int frame_count = time_diff / ms_per_frame;
							int rect_height = frame_count * delta_pos;

							note_rect[i][head[i]].h = rect_height;
							note_rect[i][head[i]].y = -rect_height;

							if(load_rect(note_tex[i],
								     note_rect[head[i]],
								     -1,
								     note_rect[i][head[i]].h,
								     (i + 1) * 100,
								     0)) {
								SDL_err();
								return;
							}
						}
						++head[i];
					}
						/* if(load_rect(note_tex[i], */
						/* 	     note_rect[head[i]], */
						/* 	     -1, */
						/* 	     note_rect[i][head[i]].h, */
						/* 	     (i + 1) * 100, */
						/* 	     0)) { */
						/* 	SDL_err(); */
						/* 	return; */
						/* } */


				}

				++object_ind;
			}

			if(update_note(win, keys, MAX_SIZE, note_rect, delta_pos, head, tail))
				goto return_;
		}

		for(int i = 0; i < keys; ++i) {
			for(unsigned char j = tail[i]; j != head[i]; ++j) {
				if(SDL_RenderCopy(win->r, note_tex[i], NULL, note_rect[i] + j)) {
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


int load_rect(SDL_Texture *tex, SDL_Rect *rect, int width, int height, int x, int y) {
	if(SDL_QueryTexture(tex, NULL, NULL, &rect->w, &rect->h)) {
		SDL_err();
		return 1;
	}

	if(height > 0)
		rect->h = height;

	if(width > 0)
		rect->w = width;


	rect->x = x;
	rect->y = y;

	return 0;
}
