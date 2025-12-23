#pragma once
#include <ncurses.h>

#include "defs.h"

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

void place_block(WINDOW* window, const float pos_x, const float pos_y);

void render_block_type(WINDOW* window, block_type type, vector2 pos);
void display_next(View* view, block_type type);

void view_refresh(View* view);
void view_clear(View* view);
