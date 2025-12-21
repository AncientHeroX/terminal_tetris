#pragma once
#include <ncurses.h>

#define BLOCK_WIDTH 8
#define BLOCK_HEIGHT 4


typedef struct
{
  WINDOW* next_block;
  WINDOW* game;
} View;


WINDOW* create_newwin(int height, int width, int starty, int startx);
void    destroy_win(WINDOW* local_win);

int view_create(View* view);
int view_destroy(View* view);

void place_block(const View* view, const float pos_x, const float pos_y);
void view_refresh(View* view);
void view_clear(View* view);
