#include "view.h"
#include <ncurses.h>

void place_block(const View* view, const float pos_x, const float pos_y)
{
  for(int x = 0; x < BLOCK_WIDTH; x++)
  {
    for(int y = 0; y < BLOCK_HEIGHT; y++)
    {
      mvwaddch(view->game, pos_y + y, pos_x + x, ACS_CKBOARD);
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

  refresh();

  int view_width, view_height;
  getmaxyx(stdscr, view_height, view_width);

  int game_window_width = BLOCK_WIDTH * 10;

  view->game = create_newwin(view_height,
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

void view_refresh(View* view)
{
  // wrefresh(view->next_block);
}
void view_clear(View* view) { werase(view->game); }
