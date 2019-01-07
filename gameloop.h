#ifndef GAMELOOP
#define GAMELOOP

struct note {
	int *times;
	char **objects;
};

void play_chart(win_ren *ren);
struct note *parse_map(int keys, FILE *fp, int early);
void gameloop(win_ren *win, int argc, char **argv);
int update_note(win_ren *win, int key_count, int note_count, SDL_Texture **tex, SDL_Rect **note, int increment, unsigned char *head, unsigned char *tail);

#endif //GAMELOOP
