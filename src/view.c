#include "view.h"
#include <ncurses.h>

// TODO (eduard):  Delete later
// #define DEBUG

void place_block(WINDOW* window, const float pos_x, const float pos_y)
{
  // horizontal movement normalization
  float new_pos_x = ((int)(pos_x / BLOCK_WIDTH) * BLOCK_WIDTH) + 1;
  for(int y = 0; y < BLOCK_HEIGHT; y++)
  {
    if(y == 0)
      mvwaddch(window, pos_y + y, new_pos_x, ACS_ULCORNER);
    else if(y == BLOCK_HEIGHT - 1)
      mvwaddch(window, pos_y + y, new_pos_x, ACS_LLCORNER);
    else
      mvwaddch(window, pos_y + y, new_pos_x, ACS_VLINE);
    int x;
    for(x = 1; x < BLOCK_WIDTH - 1; x++)
    {
      mvwaddch(window, pos_y + y, new_pos_x + x, ACS_HLINE);
    }

    if(y == 0)
      mvwaddch(window, pos_y + y, new_pos_x + x, ACS_URCORNER);
    else if(y == BLOCK_HEIGHT - 1)
      mvwaddch(window, pos_y + y, new_pos_x + x, ACS_LRCORNER);
    else
      mvwaddch(window, pos_y + y, new_pos_x + x, ACS_VLINE);
  }
}

void render_block_type(WINDOW* window, block_type type, vector2 pos)
{
  int sx = pos.x - BLOCK_WIDTH;
  int sy = pos.y - BLOCK_HEIGHT;

  int place = 1u << 15;

  for(int r = 0; r < 4; r++)
  {
    for(int c = 0; c < 4; c++)
    {
      if(type & place)
      {
        place_block(window, sx + c * BLOCK_WIDTH, sy + r * BLOCK_HEIGHT);
      }
      place >>= 1;
    }
  }
}
void display_next(View* view, block_type type)
{
  render_block_type(view->next_block,
                    type,
                    (vector2){ BLOCK_WIDTH * 2, BLOCK_HEIGHT * 1.5 });
}

WINDOW* create_newwin(int height, int width, int starty, int startx)
{
  WINDOW* local_win;

  local_win = newwin(height, width, starty, startx);
  box(local_win, 0, 0);

  wrefresh(local_win);

  return local_win;
}
void destroy_win(WINDOW* local_win)
{
  wrefresh(local_win);
  delwin(local_win);
}

int view_create(View* view)
{
  initscr();
  cbreak();
  noecho();
  curs_set(0);
  nodelay(stdscr, TRUE);
  keypad(stdscr, TRUE);
  refresh();

  int view_width = getmaxx(stdscr);

  int game_window_width  = TETRIS_WIDTH * BLOCK_WIDTH + 2;
  int game_window_height = TETRIS_HEIGHT * BLOCK_HEIGHT + 2;
  int game_window_y      = 0;
  int game_window_x      = (view_width / 2) - (game_window_width / 2);

  view->game = create_newwin(game_window_height,
                             game_window_width,
                             game_window_y,
                             game_window_x);

  view->next_block = create_newwin((BLOCK_HEIGHT * 4) + 2,
                                   (BLOCK_WIDTH * 4) + 2,
                                   game_window_y,
                                   game_window_x + game_window_width + 2);

  return 0;
}

int view_destroy(View* view)
{
  endwin();
  return 0;
}

#define redraw_border(window)                                                  \
  wborder(window,                                                              \
          ACS_VLINE,                                                           \
          ACS_VLINE,                                                           \
          ACS_HLINE,                                                           \
          ACS_HLINE,                                                           \
          ACS_ULCORNER,                                                        \
          ACS_URCORNER,                                                        \
          ACS_LLCORNER,                                                        \
          ACS_LRCORNER)

void view_refresh(View* view)
{
  redraw_border(view->game);
  wrefresh(view->game);

  redraw_border(view->next_block);
  wrefresh(view->next_block);
}

void view_clear(View* view)
{
  werase(view->next_block);
  werase(view->game);

#ifdef DEBUG
  int w_h, w_w;
  getmaxyx(view->game, w_h, w_w);
  int dark = 0;

  for(int y = 1; y < w_h - 1; y++)
  {
    if(y % BLOCK_HEIGHT == 1)
    {
      dark = !dark;
    }
    for(int x = 1; x < w_w - 1; x++)
    {
      if(x % BLOCK_WIDTH == 1)
      {
        dark = !dark;
      }
      if(dark)
      {
        mvwaddch(view->game, y, x, ' ');
      }
      else
      {
        mvwaddch(view->game, y, x, ACS_CKBOARD);
      }
    }
  }
#endif
}
