#include "view.h"
#include <ncurses.h>
#include <stdio.h>
#include <string.h>

// TODO (eduard):  Delete later
//  #define DEBUG

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

void render_block_type(WINDOW*     window,
                       block_type  type,
                       vector2     pos,
                       color_pairs pair)
{
  wattron(window, COLOR_PAIR(pair));

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

  wattroff(window, COLOR_PAIR(pair));
}
void display_next(View* view, block_type type, color_pairs pair)
{
  render_block_type(view->next_block,
                    type,
                    (vector2){ BLOCK_WIDTH * 2, BLOCK_HEIGHT * 1.5 },
                    pair);
}
void display_data(View* view, long level, long score)
{
  static char buff[64];

  sprintf(buff, "Level: %ld\n Score: %ld", level, score);
  mvwaddstr(view->score, 1, 1, buff);
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

void create_colorpairs()
{
  init_pair(J_COLOR, COLOR_WHITE, COLOR_BLUE);
  init_pair(L_COLOR, COLOR_WHITE, COLOR_RED);
  init_pair(T_COLOR, COLOR_WHITE, COLOR_MAGENTA);
  init_pair(Z_COLOR, COLOR_WHITE, COLOR_RED);
  init_pair(S_COLOR, COLOR_WHITE, COLOR_GREEN);
  init_pair(I_COLOR, COLOR_WHITE, COLOR_CYAN);
  init_pair(B_COLOR, COLOR_WHITE, COLOR_YELLOW);
}

int view_create(View* view)
{
  initscr();
  cbreak();
  noecho();
  curs_set(0);
  nodelay(stdscr, TRUE);
  keypad(stdscr, TRUE);

  start_color();
  create_colorpairs();
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

  view->score = create_newwin((BLOCK_HEIGHT * 10) + 2,
                              (BLOCK_WIDTH * 4) + 2,
                              game_window_y,
                              game_window_x - ((BLOCK_WIDTH * 4) + 4));

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

  redraw_border(view->score);
  wrefresh(view->score);
}

void view_clear(View* view)
{
  werase(view->game);
  werase(view->next_block);
  werase(view->score);

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

static const char* game_over_str[] = { "  ____    _    __  __ _____\n\
 / ___|  / \\  |  \\/  | ____|\n\
| |  _  / _ \\ | |\\/| |  _|\n\
| |_| |/ ___ \\| |  | | |__\n\
 \\____/_/   \\_\\_|  |_|_____|\n\0",
                                       "  _____     _______ ____\n\
 / _ \\ \\   / / ____|  _ \\\n\
| | | \\ \\ / /|  _| | |_) |\n\
| |_| |\\ V / | |___|  _ <\n\
 \\___/  \\_/  |_____|_| \\_\\\n\0" };

static int render_string(WINDOW* w, int x, int y, const char* str)
{
  size_t ptr = 0;
  char   c;
  int    s = 0;
  int    l = 0;

  wmove(w, y, x);
  char buff[32];

  while((c = str[ptr++]) != '\0')
  {
    if(c == '\n')
    {
      wmove(w, y + l, x);
      strncpy(buff, str + s, ((ptr - s) < 32 ? (ptr - s) : 31));

      buff[((ptr - s) < 32 ? (ptr - s) : 31)] = '\0';
      waddstr(w, buff);

      s = ptr;
      l++;
    }
  }
  return y + l;
}


void render_game_over(View* view)
{
  const int next_line = render_string(view->game, 7, 15, game_over_str[0]);
  render_string(view->game, 7, next_line, game_over_str[1]);
}
