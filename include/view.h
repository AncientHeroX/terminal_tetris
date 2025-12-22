#pragma once
#include <ncurses.h>

#define BLOCK_WIDTH 4
#define BLOCK_HEIGHT 2
#define TETRIS_HEIGHT 20
#define TETRIS_WIDTH 10

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
