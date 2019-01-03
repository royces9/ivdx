#include <SDL2/SDL.h>

#include <stdio.h>
#include <string.h>

#include <unistd.h>

#include "keybinds.h"
#include "keybinds_const.h"



#define ___KEY_TITLES___ {"[4k]",		\
			   "[5k]",		\
			   "[6k]",		\
			   "[7k]",		\
			   "[7k+1]",		\
			   "[8k]"		\
	}

#define ___FIELDS___ {4,			\
		      5,			\
		      6,			\
		      7,			\
		      8,			\
		      8				\
	}

#define ___KEY_NAMES___ {"key0",		\
			 "key1",		\
			 "key2",		\
			 "key3",		\
			 "key4",		\
			 "key5",		\
			 "key6",		\
			 "key7",		\
			 "key8"			\
	}


#define ___BIND_NAMES___ { "select",		\
			   "back",		\
			   "pause",		\
			   "scroll down",	\
			   "scroll up",		\
			   "scroll diff down",	\
			   "scroll diff up"	\
	}


char *get_cfg(void){
	char *user = getenv("HOME");
	char *cfg_path = "/.config/ivdx/ivdx.cfg";

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


	char *window_names[] = {"width",
				"height",
				"skin"
	};

	if(read_section(cfg, "[Window]", 3, window_names, (char *) window_settings, 8, read_word, buffer)) {
		printf("Error reading [Window] section.\n");
		fclose(cfg);
		free(cfg_path);
		return 1;
	}


	char *bind_names[] =  ___BIND_NAMES___;

	if(read_section(cfg, "[Settings]", 7, bind_names, (char *) kb_menu, 8, read_char, buffer)) {
		printf("Error reading [Settings] section.\n");
		return 1;
	}

	strcpy(buffer + 5, skin_dir);

	//key consts cause i wanna make this easy to read christ
	char *key_titles[] = ___KEY_TITLES___;
	unsigned char fields[] = ___FIELDS___;
	char *key_names[] = ___KEY_NAMES___;


	for(int i = 0; i < 6; ++i) {
		if(read_section(cfg, key_titles[i], fields[i], key_names, (char *) kb_game[i], 4, read_char, buffer)) {
			printf("Error reading %s section.\n", key_titles[i]);
			fclose(cfg);
			free(cfg_path);
			return 1;
		}
	}


	fclose(cfg);
	free(cfg_path);
	return 0;
}


int read_section(FILE *file, char const *const title, int fields,
		 char *field_names[], char *dest, int field_size,
		 int (*fp) (char *, char *, void *), char *buffer) {
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
		if(fp(field_names[i], buffer, dest + i * field_size))
			return 1;
	}



	return 0;
}


int read_char(char *field_name, char *buff, void *dest) {
	int field_len = strlen(field_name);

	if(strncmp(field_name, buff, field_len - 1))
		return 1;

	*(int *) dest = SDL_GetScancodeFromKey(buff[field_len + 1]);

	return 0;
}

int read_word(char *field_name, char *buff, void *dest) {
	int field_len = strlen(field_name);

	if(strncmp(field_name, buff, field_len))
		return 1;

	strcpy(*(char **) dest, buff + field_len + 1);

	return 0;
}

