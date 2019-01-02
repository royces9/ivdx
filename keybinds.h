#ifndef KEYBINDS
#define KEYBINDs

char *get_cfg(void);
int assign_binds(void);
int read_section(FILE *file, char const *const title, int fields, char *field_names[], int *kb, char *buffer);

#endif //KEYBINDS
