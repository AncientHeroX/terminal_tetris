#include "view.h"
#include <ncurses.h>

// TODO (eduard):  Delete later
#define DEBUG

void place_block(const View* view, const float pos_x, const float pos_y)
{
  for(int x = 0; x < BLOCK_WIDTH; x++)
  {
    for(int y = 0; y < BLOCK_HEIGHT; y++)
    {
      //horizontal movement normalization
      float new_pos_x = ((int)(pos_x / BLOCK_WIDTH) * BLOCK_WIDTH) + 1;

      mvwaddch(view->game, pos_y + y, new_pos_x + x, 'x');
    }
  }
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

  int view_width, view_height;
  getmaxyx(stdscr, view_height, view_width);

  int game_window_width = (BLOCK_WIDTH * 10) + 2;
  int game_window_height
    = (int)((view_height / BLOCK_HEIGHT) - 1) * BLOCK_HEIGHT + 2;

  view->game = create_newwin(game_window_height,
                             game_window_width,
                             0,
                             (view_width / 2) - (game_window_width / 2));

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
}
void view_clear(View* view) 
{ 
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

