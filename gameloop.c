#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <stdlib.h>

#include "types.h"
#include "gameloop.h"
#include "error.h"

extern int *kb_game[];

#define MAX_SIZE 256
#define BUFF_SIZE 512

int hit_line = 1080;


void play_chart(win_ren *ren) {
}


int copy_delimit(char *src, char *dest, char delimit) {
	int i = 0;
	while(*src != delimit) {
		dest = src;
		++src;
		++i;
	}

	return i;
}

struct note *get_note(char *string, struct map_timing *mp) {
	struct note *out = malloc(sizeof(*out));
	out->objects = malloc(mp->keys * sizeof(*out->objects));
	char buffer[BUFF_SIZE];

	int offset = copy_delimit(string, buffer, ',');
	string += (offset + 1);
}


struct note *parse_map(FILE *fp, struct map_timing *mp) {

	/* char buffer[BUFF_SIZE]; */

	/* while(fgets(buffer, BUFF_SIZE, fp)) { */
	/* 	if(!strncmp(buffer, "[Notes]", 7)) { */
	/* 		break; */
	/* 	} */
	/* } */

	/* memset(buffer, 0, BUFF_SIZE); */

	/* int object_count = 0; */

	/* while(fgets(buffer, BUFF_SIZE, fp)) { */
	/* 	struct note *note = get_note(buffer, mp); */
	/* } */

	int object_count = 100;
	struct note *out = calloc(object_count, sizeof(*out));

	char *temp_note = malloc(mp->keys * sizeof(*temp_note));

	set_note(out, (struct note_time){100, 0}, mp->keys, temp_note);

	set_note(out + 1, (struct note_time){200, 0}, mp->keys, temp_note);
	set_note(out + 2, (struct note_time){300, 500}, mp->keys, temp_note); 
	set_note(out + 3, (struct note_time){400, 0}, mp->keys, temp_note);
	set_note(out + 4, (struct note_time){500, 0}, mp->keys, temp_note);

	/*
	for(int i = 5, prev = 2000; i < (object_count - 1); ++i) {
		unsigned char key = 1 << (i % mp->keys);
		struct note_time temp = {prev, 50};

		set_note(out + i, temp, mp->keys, key, 2);
		prev += 50;
	}
	*/
	out[object_count - 1].objects = NULL;
	return out;
}


void gameloop(win_ren *win, int argc, char **argv) {
	struct map_timing mp;

        int quit = 0;

	//pixel / s
	int speed = 500;

	set_mp(&mp, win->fr, speed, argc);

	SDL_Event event;

	SDL_Texture **note_tex = malloc(mp.keys * sizeof(*note_tex));

	SDL_Rect **note_rect = malloc(mp.keys * sizeof(*note_rect));

	unsigned char *head = calloc(mp.keys, sizeof(*head));
	unsigned char *tail = calloc(mp.keys, sizeof(*tail));

	//int *const scancode = kb_game[mp.keys - 4];

	for(int i = 0; i < mp.keys; ++i) {
		note_tex[i] = IMG_LoadTexture(win->r, "pink.jpg");
		if(!note_tex[i]) {
			SDL_err();
			return;
		}

		note_rect[i] = calloc(MAX_SIZE, sizeof(**note_rect));
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


	int object_ind = 0;
	SDL_QueryTexture(*note_tex, NULL, NULL, NULL, &mp.default_height);

	int debug = 0;
	for(pixel_t prev = SDL_GetTicks(), start = prev; !quit; ) {
		while(SDL_PollEvent(&event)) {
			if(event.type == SDL_KEYDOWN) {
				if(event.key.keysym.scancode == SDL_SCANCODE_Q)
					goto return_;
			}
		}

		pixel_t fraction = 1;
		for(pixel_t curr;
		    (curr = SDL_GetTicks()) > prev;
		    prev += mp.ms_per_frame) {

			if(notes[object_ind].objects) {
				//amount of time elapsed
				pixel_t offset = curr - start;

				//difference between time note should appear and time elapsed
				pixel_t diff = notes[object_ind].times.start - offset;

				if(abs(diff) < mp.ms_per_frame) {
					set_rect(note_rect, notes + object_ind, &mp, head, diff);
					++object_ind;
				}
			}
			update_note(note_rect, &mp, head, tail);
		}


		for(int i = 0; i < mp.keys; ++i) {
			for(unsigned char j = tail[i]; j != head[i]; ++j) {
				note_rect[i][j].y += mp.ms_per_frame * fraction;
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
	free_notes(notes);

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


void set_rect(SDL_Rect **rect, struct note *notes, struct map_timing *mp, unsigned char *head, int diff) {
	pixel_t frames_early = diff / mp->ms_per_frame;
	pixel_t offset = frames_early * mp->delta_pos;

	for(int i = 0; i < mp->keys; ++i) {
		if(notes->objects[i] == 1) {
			pixel_t set_height = mp->default_height;

			load_rect(rect[i] + head[i],
				  100, set_height,
				  (i + 1) * 100, -(set_height + offset));

			++head[i];

		} else if(notes->objects[i] == 2) {
			pixel_t frame_count = notes->times.delta / mp->ms_per_frame;
			pixel_t set_height = frame_count * mp->delta_pos;

			load_rect(rect[i] + head[i],
				  100, set_height,
				  (i + 1) * 100, -(set_height + offset));

			++head[i];
		}
	}
}


void free_notes(struct note *notes) {
	for(int i = 0; notes[i].objects; ++i)
		free(notes[i].objects);

	free(notes);
}


void set_note(struct note *notes, struct note_time time, int key_count, char *type) {
	notes->times = time;

	notes->objects = malloc(key_count * sizeof(*notes->objects));

	for(int i = 0; i < key_count; ++i) {
		notes->objects[i] = type[i];
	}
}


void set_mp(struct map_timing *mp, int fps, int speed, int keys) {
	//number of keys
	mp->keys = keys;

	//time per frame
	// ms/frame
	mp->ms_per_frame = (pixel_t) 1000 / fps;

	//number of pixels to move down per frame
	//(pixel / s) / (frame / s)
	mp->delta_pos = (pixel_t) speed / fps;

	//ms to offset when the note should be draw w.r.t. when it hsould be hit
	//i.e. if timing is at 25ms, it should be
	//drawn earlier

	//hit line is a temp global
	mp->draw_early = (pixel_t) (hit_line * 1000) / speed;
}
