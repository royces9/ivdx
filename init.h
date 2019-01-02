#ifndef INIT
#define INIT

struct window {
	unsigned int width;
	unsigned int height;
	unsigned int flags;
};

struct render {
	unsigned int flags;
};

void startup(void);

void init_window(struct window *window);
void init_render(struct render *render);


void cleanup(SDL_Window *win, SDL_Renderer *ren);
#endif //INIT
