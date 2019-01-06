#ifndef GAMELOOP
#define GAMELOOP

struct note {
	int occ;
	SDL_Rect rect;
};

void play_chart(win_ren *ren);
void gameloop(win_ren *win, int argc, char **argv);
int update_note(win_ren *win, int key_count, int note_count, SDL_Texture **tex, struct note **note, int increment);

#endif //GAMELOOP
