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


struct note *parse_map(FILE *fp, struct map_timing *mp) {
	int object_count = 100;

	struct note *out = calloc(object_count, sizeof(*out));

	out[0].times.start = 100;
	out[0].times.end = 0;
	out[0].objects = calloc(mp->keys, sizeof(*out->objects));
	out[0].objects[0] = 1;

	out[1].times.start = 200;
	out[1].times.end = 0;
	out[1].objects = calloc(mp->keys, sizeof(*out->objects));
	out[1].objects[1] = 1;
	out[1].objects[2] = 1;

	out[2].times.start = 300;
	out[2].times.end = 800;
	out[2].objects = calloc(mp->keys, sizeof(*out->objects));
	out[2].objects[0] = 2;


	for(int i = 3, prev = 2000; i < object_count; ++i) {
		out[i].objects = calloc(mp->keys, sizeof(*out->objects));
		out[i].objects[0] = 1;

		out[i].times.start = prev;
		prev += 50;
	}

	return out;
}


void gameloop(win_ren *win, int argc, char **argv) {
	struct map_timing mp;

        int quit = 0;
	mp.keys = argc;

	//scroll speed
	//units of (pixel / s)
	int speed = 1000;

	//time per frame
	//ms/frame
	mp.ms_per_frame = 1000 / win->fr;

	//the number of pixels to move down per frame (each frame in time)
	//(pixel / s) / (frame / s)
	mp.delta_pos = speed / win->fr;

	//ms to offset when the note should be
	//drawn w.r.t. when it should be hit
	//i.e. if timing is at 25ms, it should be
	//drawn earlier

	//hit_line is a temporary global for now
	//the y coordinate of bottom of lane
	mp.draw_early = (hit_line * 1000) / speed;

	SDL_Event event;

	SDL_Texture **note_tex = malloc(mp.keys * sizeof(*note_tex));

	SDL_Rect **note_rect = malloc(mp.keys * sizeof(*note_rect));

	unsigned char *head = calloc(mp.keys, sizeof(*head));
	unsigned char *tail = calloc(mp.keys, sizeof(*tail));

	int *const scancode = kb_game[mp.keys - 4];

	for(int i = 0; i < mp.keys; ++i) {
		note_tex[i] = IMG_LoadTexture(win->r, "pink.jpg");
		if(!note_tex[i]) {
			SDL_err();
			return;
		}

		note_rect[i] = malloc(MAX_SIZE * sizeof(**note_rect));
		if(!note_rect[i]) {
			printf("Malloc failed.\n");
			return;
		}
	}

	FILE *fp;
	struct note *notes = parse_map(fp, &mp);

	if(SDL_RenderClear(win->r)) {
		SDL_err();
		goto return_;
	}	


	long lag = 0;
	int hold_ind = 0;
	int object_ind = 0;


	int default_height = 0;
	SDL_QueryTexture(*note_tex, NULL, NULL, NULL, &default_height);

	unsigned int prev = SDL_GetTicks();
	unsigned int start = prev;

	while(!quit) {
		while(SDL_PollEvent(&event)) {
			if(event.type == SDL_KEYDOWN) {
				if(event.key.keysym.scancode == SDL_SCANCODE_Q)
					goto return_;
				
				for(int i = 0; i < mp.keys; ++i) {
					if(event.key.keysym.scancode == scancode[i]) {
						head[i]++;
						break;
					}
				}
			}
		}
		
		for(unsigned int curr; (curr = SDL_GetTicks()) > prev; prev += mp.ms_per_frame) {
			unsigned int offset = curr - start;
			int diff = abs(offset - notes[object_ind].times.start);

			if(diff < mp.ms_per_frame) {
				set_rect(note_rect, notes, &mp, head, object_ind);

				++object_ind;
			}

			update_note(note_rect, &mp, head, tail);
		}

		for(int i = 0; i < mp.keys; ++i) {
			for(unsigned char j = tail[i]; j != head[i]; ++j) {
				if(SDL_RenderCopy(win->r, note_tex[i], NULL, note_rect[i] + j)) {
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

	//free the note struct

	for(int i = 0; i < mp.keys; ++i)
		SDL_DestroyTexture(*(note_tex + i));

	free(note_tex);

	for(int i = 0; i < mp.keys; ++i)
		free(*(note_rect + i));

	free(note_rect);
}


void update_note(SDL_Rect **note, struct map_timing *mp, unsigned char *head, unsigned char *tail) {
	for(int i = 0; i < mp->keys; ++i) {
		for(unsigned char j = tail[i]; j != head[i]; ++j) {
			note[i][j].y += mp->delta_pos;

			if(note[i][j].y > (hit_line + note[i][j].h))
				++tail[i];
		}
	}
}


void load_rect(SDL_Rect *rect, int width, int height, int x, int y) {
	rect->w = width;
	rect->h = height;

	rect->x = x;
	rect->y = y;
}


void set_rect(SDL_Rect **rect, struct note *notes, struct map_timing *mp, unsigned char *head, int index) {
	for(int i = 0; i < mp->keys; ++i) {
		if(notes[index].objects[i]) {
			int set_height = mp->delta_pos;

			if(notes[index].objects[i] == 2) {
				int time_diff =	notes[index].times.end - notes[index].times.start;
				int frame_count = time_diff / mp->ms_per_frame;
				int rect_height = frame_count * mp->delta_pos;
				set_height = rect_height;
			}

			rect[i][head[i]].h = set_height;
			load_rect(rect[i], 100, rect[i][head[i]].h, (i + 1) * 100, -set_height);

			++head[i];
		}
	}
}
