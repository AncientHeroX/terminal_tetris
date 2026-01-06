#pragma once
#include "defs.h"
#include "sound.h"
#include "view.h"
#include "debug.h"

#define MAX_LINE_CLEAR 4

typedef enum
{
  T_GS_RUNNING,
  T_GS_GAMEOVER,
  T_GS_PAUSED,
  T_GS_LINE_CLEARED,
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

  int   score;
  int   level;
  int   lines_cleared;
  float fall_speed;

  int    lines_to_clear[MAX_LINE_CLEAR];
  size_t clear_count;
  int    current_delete_block;
  int    animation_frame;
  long   animation_time_lapsed_us;
} game_data;

void update(game_data* data, sound_ctl* game_sound, long delta_time_ms);
void draw(View* view, game_data* data);
void init_game_state(game_data* data);

void render_block(View* view, game_data* data);
