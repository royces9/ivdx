#include <string.h>
#include <stdlib.h>

#include "skin.h"
#include "skin_const.h"

extern char *skin_dir;

int load_skin(void) {
	char *skin_cfg = skin_path();
	return 0;
}


char *skin_path(void) {
	char *cfg_path = malloc( sizeof(*cfg_path) * (strlen(skin_dir) + 9) );
	strcpy(cfg_path, skin_dir);
	strcat(cfg_path, "skin.cfg");

	return cfg_path;
}
