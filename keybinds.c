#include <SDL2/SDL.h>

#include <stdio.h>
#include <string.h>

#include <unistd.h>

#include "keybinds.h"
#include "keybind_const.h"

char *get_cfg(void){
	char *user = getenv("HOME");
	char *cfg_path = "/.config/ivdx.cfg";

	char *out = malloc( (strlen(user) + strlen(cfg_path) + 1) * sizeof(*out) );

	strcpy(out, user);
	free(user);
	strcat(out, cfg_path);
	
	return out;
}

#define BUFF_SIZE 64

int assign_binds(void) {
	char *cfg_path = 

#ifdef RELEASE
	get_cfg();
#else
	malloc(9 * sizeof(*cfg_path));
	cfg_path[0] = 'i'; cfg_path[1] = 'v';
	cfg_path[2] = 'd'; cfg_path[3] = 'x';
	cfg_path[4] = '.'; cfg_path[5] = 'c';
	cfg_path[6] = 'f'; cfg_path[7] = 'g';
	cfg_path[8] = 0;
#endif

	FILE *cfg = fopen(cfg_path, "r");
	if(!cfg) {
		printf("Could not open config.\n");
		return 1;
	}

	char buffer[BUFF_SIZE];

	char *bind_names[] = { "select",
			       "back",
			       "pause",
			       "scroll down",
			       "scroll up",
			       "scroll diff down",
			       "scroll diff up"
	};

	if(read_section(cfg, "[Settings]", 7, bind_names, kb_menu[0], buffer)) {
		printf("Error reading [Settings] section.\n");
		return 1;
	}

	char *key_titles[] = {"[4k]",
			      "[5k]",
			      "[6k]",
			      "[7k]",
			      "[7k+1]",
			      "[8k]"
	};

	unsigned char fields[] = {4,
				 5,
				 6,
				 7,
				 8,
				 8
	};


	char *key_names[] = {"key0",
			   "key1",
			   "key2",
			   "key3",
			   "key4",
			   "key5",
			   "key6",
			   "key7",
			   "key8"
	};
	for(int i = 0; i < 6; ++i) {
		if(read_section(cfg, key_titles[i], fields[i], key_names, kb_game[i], buffer)) {
			printf("Error reading %s section.\n", key_titles[i]);
			return 1;
		}
	}


	fclose(cfg);
	free(cfg_path);
	return 0;
}


int read_section(FILE *file, char const *const title, int fields, char *field_names[], int *kb, char *buffer) {
	memset(buffer, 0, BUFF_SIZE);

	int len = strlen(title);
	while(fgets(buffer, BUFF_SIZE, file)) {
		if(buffer[0] == '\n')
			continue;

		if(strncmp(title, buffer, len))
			return 1;

		break;
	}
	
	for(int i = 0; i < fields; ++i) {
		fgets(buffer, BUFF_SIZE, file);
		int field_len = strlen(field_names[i]);

		if(strncmp(field_names[i], buffer, field_len))
			return 1;

		kb[i] = SDL_GetScancodeFromKey(buffer[field_len + 1]);
	}

	return 0;
}
