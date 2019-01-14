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


struct map_timing {
	int ms_per_frame;
	int delta_pos;
	int draw_early;
	int keys;
};

void play_chart(win_ren *ren);
struct note *parse_map(FILE *fp, struct map_timing *mp);
void gameloop(win_ren *win, int argc, char **argv);
void update_note(SDL_Rect **note, struct map_timing *mp, unsigned char *head, unsigned char *tail);
void load_rect(SDL_Rect *rect, int width, int height, int x, int y);
void set_rect(SDL_Rect **rect, struct note *notes, struct map_timing *mp, unsigned char *head, int index);

#endif //GAMELOOP
