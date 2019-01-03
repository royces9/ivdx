#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <stdlib.h>

#include "gameloop.h"
#include "error.h"

extern int *kb_game[];

#define MAX_SIZE 128

struct note {
	int occ;
	SDL_Rect rect;
};


void play_chart(SDL_Window *win, SDL_Renderer *ren);

void gameloop(SDL_Window *win, SDL_Renderer *ren, int argc, char **argv) {
        int quit = 0;
	int keys = argc;
	int speed = 10;

	SDL_Event event;
	SDL_Texture **tex = malloc(keys * sizeof(*tex));
	struct note **rect = malloc(keys * sizeof(*rect));

	int *early = calloc(keys, sizeof(*early));
	int *late = calloc(keys, sizeof(*late));

	int *scancode = kb_game[keys - 4];

	for(int i = 0; i < keys; ++i) {
		*(tex + i) = IMG_LoadTexture(ren, "pink.jpg");
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
		while(SDL_PollEvent(&event));

		if(state[SDL_SCANCODE_Q])
			return;

		if(SDL_RenderClear(ren)) {
			SDL_err();
			return;
		}

		for(int i = 0; i < keys; ++i) {
			if(state[scancode[i]]) {
				rect[i][late[i]++].occ = 1;

				if(late[i] >= MAX_SIZE)
					late[i] = 0;
			}

			for(int j = 0; j < MAX_SIZE; ++j) {
				if(rect[i][j].occ) {
					if(SDL_RenderCopy(ren, *(tex + i), NULL, &rect[i][j].rect)) {
						SDL_err();
						return;
					}
					rect[i][j].rect.y += speed;
					if(rect[i][j].rect.y > 1080) {
						rect[i][j].rect.y = 0;
						rect[i][j].occ = 0;
						++early[i];
						if(early[i] >= MAX_SIZE)
							early[i] = 0;
					}
				}
			}
		}
		SDL_RenderPresent(ren);

		SDL_Delay(1);
	}
	
	free(scancode);
	free(early);
	free(late);
	for(int i = 0; i < keys; ++i)
		SDL_DestroyTexture(*(tex + i));

	free(tex);

	for(int i = 0; i < MAX_SIZE; ++i)
		free(*(rect + i));

	free(rect);
}
