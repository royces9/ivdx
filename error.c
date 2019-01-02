#include <SDL2/SDL.h>

#include "error.h"

void SDL_err(void) {
	printf("Error: %s\n", SDL_GetError());
}
