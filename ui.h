#ifndef _UI_
#define _UI_

void draw_static(SDL_Renderer *ren, char *file, int x, int y, int w, int h);
void draw_move(SDL_Renderer *ren, SDL_Texture *tex, SDL_Rect rect, int x, int y, int w, int h, int vx, int vy, int t);

#endif //_UI_
