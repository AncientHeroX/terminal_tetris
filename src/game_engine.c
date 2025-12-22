#include <math.h>
#include <ncurses.h>

#include "game_engine.h"
#include "view.h"

void update(game_data* data)
{
  int c = getch();
  switch(c)
  {
  case KEY_RIGHT:
  case 'd':
    data->falling_piece.x = fminf(data->falling_piece.x + BLOCK_WIDTH,
                                  (TETRIS_WIDTH - 1) * BLOCK_WIDTH);
    break;

  case KEY_LEFT:
  case 'a':
    data->falling_piece.x = fmaxf(data->falling_piece.x - BLOCK_WIDTH, 1);
    break;
  }

  data->falling_piece.y += data->fall_speed;
}

void draw(View* view, game_data* data)
{
  place_block(view, data->falling_piece.x, data->falling_piece.y, 'X');
}

float calculate_fall_speed(long level) { return (1.0f / 10000) * level; }
void  init_game_state(game_data* data)
{
  data->level      = 1;
  data->fall_speed = calculate_fall_speed(data->level);

  data->falling_piece = (vector2){ .x = BLOCK_WIDTH * 4, .y = 0 };
}
