#ifndef GAMELOOP
#define GAMELOOP

typedef double pixel_t;
typedef unsigned char key_flag
struct note_time {
	int start;
	int end;
};


struct note {
	struct note_time times;
	char *objects;
};


struct note_list {
	struct note *note;
	struct note_list *next;
}

struct map_timing {
	//ms per frame to draw
	pixel_t ms_per_frame;

	//pixels to move per frame
	pixel_t delta_pos;

	//how many ms to draw early
	pixel_t draw_early;

	//height of the sprites
	int default_height;
	
	//number of keys
	int keys;
};

void play_chart(win_ren *ren);
struct note *parse_map(FILE *fp, struct map_timing *mp);
void gameloop(win_ren *win, int argc, char **argv);
void update_note(SDL_Rect **note, struct map_timing *mp, unsigned char *head, unsigned char *tail, pixel_t fraction);
void load_rect(SDL_Rect *rect, int width, int height, int x, int y);
void set_rect(SDL_Rect **rect, struct note *notes, struct map_timing *mp, unsigned char *head, int index, int diff);
void free_notes(struct note *notes);
void set_note(struct note *notes, struct note_time note, int key_count, key_flag key, int type);

#endif //GAMELOOP

