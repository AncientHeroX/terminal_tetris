#pragma once
#include "view.h"


typedef enum
{
  J = 17600,
  L = 17504,
  T = 19968,
  Z = 50688,
  S = 27648,
  I = 34952,
  B = 52224
} block_type;

typedef enum
{
  UP,
  DOWN,
  LEFT,
  RIGHT
} rotation;

typedef struct
{
  float x, y;
} vector2;


typedef struct
{
  vector2    falling_piece;
  block_type falling_piece_type;

  vector2 lower_pool[TETRIS_WIDTH][TETRIS_HEIGHT];

  long  level;
  float fall_speed;
} game_data;

void update(game_data* data);
void draw(View* view, game_data* data);
void init_game_state(game_data* data);

void render_block(View* view, game_data* data);
