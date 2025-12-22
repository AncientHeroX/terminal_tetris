#pragma once
#include "view.h"

typedef struct
{
  float x;
  float y;
} vector2;

typedef struct
{
  vector2 falling_piece;
  vector2 lower_pool[TETRIS_WIDTH][TETRIS_HEIGHT];

  long  level;
  float fall_speed;
} game_data;

void update(game_data* data);
void draw(View* view, game_data* data);
void init_game_state(game_data* data);
