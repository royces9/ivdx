#ifndef KEYBINDS
#define KEYBINDs

char *get_cfg(void);
int assign_binds(void);
int read_char(char *field_name, char *buff, void *dest);
int read_word(char *field_name, char *buff, void *dest);
int read_section(FILE *file, char const *const title, int fields,
		 char *field_names[], char *dest, int field_size,
		 int (*fp) (char *, char *, void *), char *buffer);

#endif //KEYBINDS
