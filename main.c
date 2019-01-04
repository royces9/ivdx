#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

#include "types.h"
#include "mainloop.h"
#include "init.h"
#include "ui.h"
#include "error.h"
#include "keybinds.h"
#include "skin.h"


int main(int argc, char **argv) {

	startup();

	if(assign_binds()) {
		printf("Failed to assign binds.\n");
		exit(1);
	}

	if(load_skin()) {
		printf("Failed to load skin.\n");
		exit(1);
	}

	win_ren w;
	start_window(&w);

	mainloop(&w, argc, argv);

	cleanup(&w);
	SDL_Quit();
	return 0;
}
