#include <math.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "game_engine.h"
#include "view.h"
static block_type block_types[]
  = { 17600, 17504, 19968, 50688, 27648, 34952, 52224 };

uint16_t rotate(uint16_t x)
{
  uint16_t r0 = x & 0xF;
  uint16_t r1 = (x >> 4) & 0xF;
  uint16_t r2 = (x >> 8) & 0xF;
  uint16_t r3 = (x >> 12) & 0xF;

  uint16_t nr0 = ((r3 >> 0) & 1) | (((r2 >> 0) & 1) << 1)
                 | (((r1 >> 0) & 1) << 2) | (((r0 >> 0) & 1) << 3);
  uint16_t nr1 = ((r3 >> 1) & 1) | (((r2 >> 1) & 1) << 1)
                 | (((r1 >> 1) & 1) << 2) | (((r0 >> 1) & 1) << 3);
  uint16_t nr2 = ((r3 >> 2) & 1) | (((r2 >> 2) & 1) << 1)
                 | (((r1 >> 2) & 1) << 2) | (((r0 >> 2) & 1) << 3);
  uint16_t nr3 = ((r3 >> 3) & 1) | (((r2 >> 3) & 1) << 1)
                 | (((r1 >> 3) & 1) << 2) | (((r0 >> 3) & 1) << 3);

  return nr0 | (nr1 << 4) | (nr2 << 8) | (nr3 << 12);
}

void new_block(game_data* data)
{
  int r = rand() % 7;

  data->falling_piece_type = block_types[r];
  data->falling_piece      = (vector2){ .x = BLOCK_WIDTH * 4, .y = 0 };
}

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
  case 'c':
    data->falling_piece_type = rotate(data->falling_piece_type);
    break;
  case ' ':
    data->falling_piece.y += 1;
    break;
  }

  data->falling_piece.y += data->fall_speed;

  float sx = data->falling_piece.x - BLOCK_WIDTH;
  float sy = data->falling_piece.y - BLOCK_HEIGHT;

  size_t ptr   = 0;
  int    place = 1u << 15;
  int    save  = 0;

  for(int r = 0; r < 4; r++)
  {
    for(int c = 0; c < 4; c++)
    {
      if(data->falling_piece_type & place)
      {
        data->falling_piece_blocks[ptr++] = (vector2){ c, r };

        if(sx + (c * BLOCK_WIDTH) < 0)
          data->falling_piece.x = (int)(data->falling_piece.x + 1);
        if(sx + (c * BLOCK_WIDTH) > (TETRIS_WIDTH - 1) * BLOCK_WIDTH)
          data->falling_piece.x = (int)(data->falling_piece.x - 1);
        if(sy + (r * BLOCK_HEIGHT) > TETRIS_HEIGHT * BLOCK_HEIGHT)
          save = 1;
      }

      place >>= 1;
    }
  }
  sx = data->falling_piece.x - BLOCK_WIDTH;

  for(int b = 0; b < 4; b++)
  {
    vector2* cb = &data->falling_piece_blocks[b];

    int px = sx + (cb->x * BLOCK_WIDTH);
    int py = sy + (cb->y * BLOCK_HEIGHT);

    if(save)
    {
      data->lower_pool[(int)(px / BLOCK_WIDTH)][(int)(py / BLOCK_HEIGHT) - 1]
        = 1;
    }
  }
  if(save)
    new_block(data);
}

void draw(View* view, game_data* data)
{
  render_block(view, data);

  for(int y = 0; y < TETRIS_HEIGHT; y++)
  {
    for(int x = 0; x < TETRIS_WIDTH; x++)
    {
      if(data->lower_pool[x][y] == 1)
      {
        place_block(view, x * BLOCK_WIDTH, 1 + (y * BLOCK_HEIGHT));
      }
    }
  }
}

float calculate_fall_speed(long level) { return (1.0f / 10000) * level; }

void init_game_state(game_data* data)
{
  srand(time(NULL));
  data->level      = 1;
  data->fall_speed = calculate_fall_speed(data->level);

  data->falling_piece      = (vector2){ .x = BLOCK_WIDTH * 4, .y = 0 };
  data->falling_piece_type = L;

  for(int i = 0; i < TETRIS_WIDTH; i++)
  {
    memset(data->lower_pool[i], 0, sizeof(int) * TETRIS_HEIGHT);
  }
}

void render_block(View* view, game_data* data)
{

  int sx = data->falling_piece.x - BLOCK_WIDTH;
  int sy = data->falling_piece.y - BLOCK_HEIGHT;

  for(int b = 0; b < 4; b++)
  {
    place_block(view,
                sx + data->falling_piece_blocks[b].x * BLOCK_WIDTH,
                sy + data->falling_piece_blocks[b].y * BLOCK_HEIGHT);
  }
}
