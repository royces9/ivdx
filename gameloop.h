#ifndef GAMELOOP
#define GAMELOOP

struct note_time {
	int start;
	int end;
};

struct note {
	struct note_time times;
	char *objects;
};

void play_chart(win_ren *ren);
struct note *parse_map(int keys, FILE *fp, int early, int frame_time);
void gameloop(win_ren *win, int argc, char **argv);
int update_note(win_ren *win, int key_count, int note_count, SDL_Rect **note, int increment, unsigned char *head, unsigned char *tail);
int load_rect(SDL_Texture *tex, SDL_Rect *rect, int width, int height, int x, int y);

#endif //GAMELOOP
