#include <assert.h>
#include <math.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "game_engine.h"
#include "sound.h"
#include "view.h"

#define MAX_LEVEL 15
#define SCORE_MAX 1e9

static float calculate_fall_speed(int level)
{
  return 1e-6f * exp((0.2f) * level);
}

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
        data->color_pool[sx + c][sy + r] = data->pair;
      }
      place >>= 1;
    }
  }
}
static void update_level(game_data* data, int lines_cleared)
{
  if(lines_cleared <= 0)
    return;

  data->lines_cleared += lines_cleared;
  if(data->lines_cleared >= 10)
  {
    data->lines_cleared %= 10;
    data->level = data->level + 1 > MAX_LEVEL ? MAX_LEVEL : data->level + 1;
  }

  data->fall_speed = calculate_fall_speed(data->level);
}

static void update_score(game_data* data, int lines_cleared)
{
  static int line_scores[] = { 40, 100, 300, 1200 };

  if(lines_cleared <= 0)
    return;

  int new_score = data->score + line_scores[lines_cleared - 1] * data->level;

  data->score = new_score < SCORE_MAX ? new_score : SCORE_MAX - 1;
}

void shift_line(game_data* data)
{
  // Shift
  int next_blank = 0;
  int curr_line  = data->lines_to_clear[next_blank++];

  for(int y = curr_line - 1; y >= 0; y--)
  {
    if(y == data->lines_to_clear[next_blank])
    {
      next_blank++;
      continue;
    }
    assert(curr_line > y);

    for(int x = 0; x < TETRIS_WIDTH; x++)
    {
      data->lower_pool[x][curr_line] = data->lower_pool[x][y];
      data->lower_pool[x][y]         = data->lower_pool[x][y];

      data->color_pool[x][curr_line] = data->color_pool[x][y];
      data->color_pool[x][y]         = data->color_pool[x][y];
    }
    curr_line--;
  }

  for(int i = 0; i < MAX_LINE_CLEAR; i++)
  {
    data->lines_to_clear[i] = 0;
  }
}


#define FRAME_INTERVAL_US (1 / 100000.0f)
static void update_line_removal(game_data* data, long delta_time_us)
{
  data->animation_time_lapsed_us += delta_time_us;

  int current_frame = data->animation_time_lapsed_us * FRAME_INTERVAL_US;

  bool is_next_frame = current_frame > data->animation_frame;

  if(!is_next_frame)
    return;

  if(data->current_delete_block < TETRIS_WIDTH)
  {
    data->animation_frame++;
    int delete_pair = (TETRIS_WIDTH - data->current_delete_block) - 1;
    varlog("%d\n", delete_pair);


    for(int y = data->clear_count - 1; y >= 0; y--)
    {
      if(data->lines_to_clear[y] != 0)
      {
        data->color_pool[data->current_delete_block][data->lines_to_clear[y]]
          = LINE_CLEARED_COLOR;
        data->color_pool[delete_pair][data->lines_to_clear[y]]
          = LINE_CLEARED_COLOR;
      }
    }

    data->current_delete_block++;
  }
  else
  {
    shift_line(data);
    data->animation_time_lapsed_us = 0;
    data->current_delete_block     = (TETRIS_WIDTH / 2) - 1;
    varlog("%d\n", data->current_delete_block);
    data->animation_frame = 0;
    data->state           = T_GS_RUNNING;
  }
}

static int check_line(game_data* data)
{
  size_t lines_cleared = 0;

  for(int y = TETRIS_HEIGHT - 1; y >= 0; y--)
  {
    int count = 0;
    for(int x = 0; x < TETRIS_WIDTH; x++)
    {
      if(data->lower_pool[x][y])
        count++;
    }
    if(count == TETRIS_WIDTH)
      data->lines_to_clear[lines_cleared++] = y;
  }

  if(lines_cleared > 0)
  {
    data->state = T_GS_LINE_CLEARED;
  }

  data->clear_count = lines_cleared;

  update_level(data, lines_cleared);
  update_score(data, lines_cleared);

  return lines_cleared > 0;
}

static block_type block_types[] = { J, L, T, Z, S, I, B };

uint16_t rotate(const uint16_t x)
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
          return 0;

        if(xstart + c >= TETRIS_WIDTH)
          return 2;
        if(xstart + c < 0)
          return 1;

        if(data->lower_pool[xstart + c][ystart + r])
          return (xstart + c > pos.x) + 1;
      }

      place >>= 1;
    }
  }
  return 0;
}

static void new_block(game_data* data)
{
  int r = rand() % 7;

  data->falling_piece_type = data->next_piece;
  data->pair               = get_type_color(data->falling_piece_type);

  data->next_piece = block_types[r];
  data->next_pair  = get_type_color(data->next_piece);

  data->falling_piece = (vector2){ .x = BLOCK_WIDTH * 4, .y = 3 };
}

static void
running_update(game_data* data, sound_ctl* game_sound, long delta_time_us)
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
  {
    const uint16_t old       = data->falling_piece_type;
    data->falling_piece_type = rotate(data->falling_piece_type);
    int col;
    int tries = 0;

    while((col = check_horizontal_collision(data->falling_piece, data)) != 0
          && tries++ < 10)
    {
      switch(col)
      {
      case 1:
        change.x += BLOCK_WIDTH;
        break;
      case 2:
        change.x -= BLOCK_WIDTH;
        break;
      }
    }

    if(tries >= 10)
    {
      data->falling_piece_type = old;
      change.x                 = 0;
    }
  }
  break;
  case 's':
  case KEY_DOWN:
    change.y = 2;
    break;
  case ' ':
  {
    data->falling_piece.y++;
    while(check_lower_collision(data->falling_piece, data) == 0)
    {
      data->falling_piece.y++;
    }

    data->falling_piece.y -= 1;
    play_sound(game_sound, SOUND_PLACE_BLOCK);
    lock_piece(data);
    new_block(data);

    if(check_line(data))
    {
      play_sound(game_sound, SOUND_LINE_CLEAR);
    }
    return;
  }
  break;
  case 'p':
    if(data->state != T_GS_PAUSED)
      data->state = T_GS_PAUSED;
    else
      data->state = T_GS_RUNNING;
    break;
  }

  if(data->state == T_GS_PAUSED)
    return;


  change.y += data->fall_speed * delta_time_us;

  if(check_horizontal_collision(vec2add(&data->falling_piece, &change), data))
  {
    change.x = 0;
  }

  if(check_lower_collision(vec2add(&data->falling_piece, &change), data))
  {
    if(data->falling_piece.y <= 3)
    {
      data->state = T_GS_GAMEOVER;
      play_sound(game_sound, SOUND_GAME_OVER);
      return;
    }

    change.y -= data->fall_speed;
    while((check_lower_collision(vec2add(&data->falling_piece, &change), data))
          != 0)
    {
      change.y -= data->fall_speed;
    }
    data->falling_piece.y += change.y;

    play_sound(game_sound, SOUND_PLACE_BLOCK);
    lock_piece(data);
    new_block(data);

    if(check_line(data))
    {
      play_sound(game_sound, SOUND_LINE_CLEAR);
    }
  }
  else
  {
    data->falling_piece = vec2add(&data->falling_piece, &change);
  }
}

void update(game_data* data, sound_ctl* game_sound, long delta_time_us)
{
  switch(data->state)
  {
  case T_GS_PAUSED:
  case T_GS_RUNNING:
    running_update(data, game_sound, delta_time_us);
    break;
  case T_GS_LINE_CLEARED:
    update_line_removal(data, delta_time_us);
    break;
  default:
    return;
  }
}
static void running_draw(View* view, game_data* data)
{
  render_block(view, data);
  display_next(view, data->next_piece, data->next_pair);
  display_data(view, data->level, data->score);

  for(int y = 0; y < TETRIS_HEIGHT; y++)
  {
    for(int x = 0; x < TETRIS_WIDTH; x++)
    {
      if(data->lower_pool[x][y] == 1)
      {
        wattron(view->game, COLOR_PAIR(data->color_pool[x][y]));
        place_block(view->game, x * BLOCK_WIDTH, 1 + (y * BLOCK_HEIGHT));
        wattroff(view->game, COLOR_PAIR(data->color_pool[x][y]));
      }
    }
  }
}

void draw(View* view, game_data* data)
{
  switch(data->state)
  {
  case T_GS_GAMEOVER:
    running_draw(view, data);
    render_game_over(view, data->score);
    break;
  default:
    running_draw(view, data);
    break;
  }
}


void init_game_state(game_data* data)
{
  srand(time(NULL));
  data->level         = 1;
  data->score         = 0;
  data->lines_cleared = 0;
  data->fall_speed    = calculate_fall_speed(data->level);
  data->state         = T_GS_RUNNING;

  data->current_delete_block     = TETRIS_WIDTH / 2;
  data->animation_time_lapsed_us = 0;
  data->animation_frame          = 0;

  memset(data->lines_to_clear, 0, sizeof(int) * MAX_LINE_CLEAR);
  data->clear_count = 0;

  data->next_piece = block_types[(int)(rand() % 7)];
  new_block(data);

  for(int i = 0; i < TETRIS_WIDTH; i++)
  {
    memset(data->lower_pool[i], 0, sizeof(int) * TETRIS_HEIGHT);
  }
}

void render_block(View* view, game_data* data)
{
  render_block_type(view->game,
                    data->falling_piece_type,
                    data->falling_piece,
                    data->pair);
}
