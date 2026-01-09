#pragma once
#include <ncurses.h>

#include "defs.h"
#include "save.h"

#define BLOCK_WIDTH 4
#define BLOCK_HEIGHT 2
#define TETRIS_HEIGHT 20
#define TETRIS_WIDTH 10

typedef struct
{
  WINDOW* next_block;
  WINDOW* score;
  WINDOW* game;
} View;


WINDOW* create_newwin(int height, int width, int starty, int startx);
void    destroy_win(WINDOW* local_win);

int view_create(View* view);
int view_destroy(View* view);

void place_block(WINDOW* window, const float pos_x, const float pos_y);

void render_block_type(WINDOW*     window,
                       block_type  type,
                       vector2     pos,
                       color_pairs pair);
void display_next(View* view, block_type type, color_pairs pair);
void display_data(View* view, long level, long score);

void view_refresh(View* view);
void view_clear(View* view);

void render_game_over(View* restrict view,
                      const int  score,
                      const bool draw_keyboard,
                      const char* restrict name,
                      const char keyboard_pointer);
void render_leader_board(View* view, const leaderboard_t* leaderboard, int y);
