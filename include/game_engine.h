#pragma once
#include "defs.h"
#include "sound.h"
#include "view.h"

typedef enum
{
  T_GS_RUNNING,
  T_GS_GAMEOVER,
} game_states;

typedef struct
{
  game_states state;

  vector2     falling_piece;
  block_type  falling_piece_type;
  color_pairs pair;

  block_type  next_piece;
  color_pairs next_pair;

  int         lower_pool[TETRIS_WIDTH][TETRIS_HEIGHT];
  color_pairs color_pool[TETRIS_WIDTH][TETRIS_HEIGHT];

  long  score;
  long  level;
  long  lines_cleared;
  float fall_speed;

  long points;
} game_data;

void update(game_data* data, sound_ctl* game_sound);
void draw(View* view, game_data* data);
void init_game_state(game_data* data);

void render_block(View* view, game_data* data);
