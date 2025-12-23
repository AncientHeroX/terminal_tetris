#include <math.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "game_engine.h"
#include "view.h"

static vector2 vec2add(const vector2* restrict a, vector2* restrict b)
{
  return (vector2){ a->x + b->x, a->y + b->y };
}
static void lock_piece(game_data* data)
{
  int place = 1u << 15;

  int sx = (data->falling_piece.x - BLOCK_WIDTH) / BLOCK_WIDTH;
  int sy = (data->falling_piece.y - BLOCK_HEIGHT) / BLOCK_HEIGHT;

  for(int r = 0; r < 4; r++)
  {
    for(int c = 0; c < 4; c++)
    {
      if(data->falling_piece_type & place)
      {
        data->lower_pool[sx + c][sy + r] = 1;
      }
      place >>= 1;
    }
  }
}

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


static int check_lower_collision(vector2 pos, game_data* data)
{
  int   place  = 1u << 15;
  float ystart = (pos.y - BLOCK_HEIGHT);
  float xstart = (pos.x - BLOCK_WIDTH);
  float x, y;

  for(int r = 0; r < 4; r++)
  {
    for(int c = 0; c < 4; c++)
    {
      if(data->falling_piece_type & place)
      {
        y = (ystart + r * BLOCK_HEIGHT);
        x = (xstart + c * BLOCK_WIDTH);

        if(y >= TETRIS_HEIGHT * BLOCK_HEIGHT)
          return 1;

        if(data->lower_pool[(int)(x / BLOCK_WIDTH)][(int)(y / BLOCK_HEIGHT)])
          return 1;
      }

      place >>= 1;
    }
  }
  return 0;
}
static int check_horizontal_collision(vector2 pos, game_data* data)
{
  int place  = 1u << 15;
  int ystart = (pos.y - BLOCK_HEIGHT) / BLOCK_HEIGHT;
  int xstart = (pos.x - BLOCK_WIDTH) / BLOCK_WIDTH;

  for(int r = 0; r < 4; r++)
  {
    for(int c = 0; c < 4; c++)
    {
      if(data->falling_piece_type & place)
      {
        if(ystart + r >= TETRIS_HEIGHT || ystart + r < 0)
          return 1;
        if(xstart + c >= TETRIS_WIDTH || xstart + c < 0)
          return 1;

        if(data->lower_pool[xstart + c][ystart + r])
          return 1;
      }

      place >>= 1;
    }
  }
  return 0;
}

static void new_block(game_data* data)
{
  int r = rand() % 7;

  data->falling_piece_type = block_types[r];
  data->falling_piece      = (vector2){ .x = BLOCK_WIDTH * 4, .y = 0 };
}

void update(game_data* data)
{
  int c = getch();

  vector2 change = { 0 };
  switch(c)
  {
  case KEY_RIGHT:
  case 'd':
    change.x = BLOCK_WIDTH;
    break;

  case KEY_LEFT:
  case 'a':
    change.x = -BLOCK_WIDTH;
    break;
  case 'c':
    data->falling_piece_type = rotate(data->falling_piece_type);
    break;
  case ' ':
    change.y = 2;
    break;
  }

  change.y += data->fall_speed;

  if(check_horizontal_collision(vec2add(&data->falling_piece, &change), data))
  {
    change.x = 0;
  }

  if(check_lower_collision(vec2add(&data->falling_piece, &change), data))
  {
    lock_piece(data);
    new_block(data);
  }
  else
  {
    data->falling_piece = vec2add(&data->falling_piece, &change);
  }
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

  int place = 1u << 15;

  for(int r = 0; r < 4; r++)
  {
    for(int c = 0; c < 4; c++)
    {
      if(data->falling_piece_type & place)
      {
        place_block(view, sx + c * BLOCK_WIDTH, sy + r * BLOCK_HEIGHT);
      }
      place >>= 1;
    }
  }
}
