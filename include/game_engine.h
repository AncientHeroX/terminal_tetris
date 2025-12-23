#pragma once
#include "defs.h"
#include "view.h"

typedef struct
{
  vector2    falling_piece;
  block_type falling_piece_type;
  block_type next_piece;

  int lower_pool[TETRIS_WIDTH][TETRIS_HEIGHT];

  long  level;
  long  lines_cleared;
  float fall_speed;

  long points;
} game_data;

void update(game_data* data);
void draw(View* view, game_data* data);
void init_game_state(game_data* data);

void render_block(View* view, game_data* data);
