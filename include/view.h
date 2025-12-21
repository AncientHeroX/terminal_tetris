#pragma once
#include <ncurses.h>

#define BLOCK_WIDTH 8
#define BLOCK_HEIGHT 4


typedef struct
{
  WINDOW* gameWindow;
} View;


WINDOW* create_newwin(int height, int width, int starty, int startx);
void    destroy_win(WINDOW* local_win);

int view_create(View* view);
int view_destroy(View* view);
