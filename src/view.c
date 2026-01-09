#include "view.h"
#include "debug.h"
#include "numbers.h"
#include "save.h"
#include <assert.h>
#include <curses.h>
#include <ncurses.h>
#include <stdio.h>
#include <string.h>


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
  init_pair(LINE_CLEARED_COLOR, COLOR_WHITE, COLOR_WHITE);
  init_pair(KEY_SELECT_COLOR, COLOR_BLACK, COLOR_WHITE);
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
}

static const char* game_over_str[] = { " ____    _    __  __ _____\n\
 / ___|  / \\  |  \\/  | ____|\n\
| |  _  / _ \\ | |\\/| |  _|\n\
| |_| |/ ___ \\| |  | | |__\n\
 \\____/_/   \\_\\_|  |_|_____|\n\0",
                                       " _____     _______ ____\n\
 / _ \\ \\   / / ____|  _ \\\n\
| | | \\ \\ / /|  _| | |_) |\n\
| |_| |\\ V / | |___|  _ <\n\
 \\___/  \\_/  |_____|_| \\_\\\n\0" };

static int render_string(WINDOW* w, int x, int y, const char* str)
{
  size_t ptr = 0;
  char   c;

  int maxx = getmaxx(w);

  wmove(w, y, 0);
  int py = y, px = 0;

  for(px = 0; px <= maxx; px++)
  {
    if(px < x)
    {
      waddch(w, ' ');
      continue;
    }

    c = str[ptr++];
    if(c == '\n')
    {
      for(int j = x + 1; j <= maxx; j++)
      {
        waddch(w, ' ');
      }
      wmove(w, ++py, 0);
      px = 0;
      continue;
    }
    else if(c == '\0')
    {
      for(int j = x + 1; j <= maxx; j++)
      {
        waddch(w, ' ');
      }
      break;
    }
    else
    {
      waddch(w, c);
    }
  }
  return py;
}
static void w_print_divider(WINDOW* win, const int y)
{
  wmove(win, y, 1);
  for(int x = 0; x < TETRIS_WIDTH * BLOCK_WIDTH; x++)
  {
    waddch(win, '-');
  }
}
static void clear_line(WINDOW* win, const int y)
{
  wmove(win, y, 1);
  for(int x = 0; x < TETRIS_WIDTH * BLOCK_WIDTH; x++)
  {
    waddch(win, ' ');
  }
}

#define GET_CENTER_IN_GAMEWINDOW(item_width)                                   \
  (int)((TETRIS_WIDTH * BLOCK_WIDTH) / 2) - ((item_width) / 2)

static int w_print_name(WINDOW* win, const char* name, int y)
{
  w_print_divider(win, y++);

  clear_line(win, y);

  const int start = GET_CENTER_IN_GAMEWINDOW(5);
  wmove(win, y++, start);
  for(int c = 0; c < 3; c++)
  {
    if(name[c] == '\0')
    {
      waddch(win, ACS_CKBOARD);
    }
    else
    {
      waddch(win, name[c]);
    }
    waddch(win, ' ');
  }

  w_print_divider(win, y++);
  return y;
}
static int w_print_keyboard(WINDOW* win, int y, const char keyboard_pointer)
{
  assert(keyboard_pointer >= 'A' && keyboard_pointer <= CONFIRM_CHAR_KEY);

  int start_x = GET_CENTER_IN_GAMEWINDOW(
    KEYBOARD_BUTTONS_PER_LINE * KEYBOARD_BUTTON_WIDTH + 2);
  int x = start_x + 1;

  clear_line(win, y);
  wmove(win, y, x);
  for(char c = 'A'; c <= 'Z'; c++)
  {
    char str[6];
    sprintf(str, "[ %c ]", c);

    if(c == keyboard_pointer)
    {
      wattron(win, COLOR_PAIR(KEY_SELECT_COLOR));
      waddnstr(win, str, 5);
      wattroff(win, COLOR_PAIR(KEY_SELECT_COLOR));
    }
    else
    {
      waddnstr(win, str, 5);
    }

    x += KEYBOARD_BUTTON_WIDTH;
    if(x > KEYBOARD_BUTTONS_PER_LINE * KEYBOARD_BUTTON_WIDTH)
    {
      y += 1;

      x = start_x + 1;
      clear_line(win, y);
      wmove(win, y, x);
    }
  }

  if(DEL_CHAR_KEY == keyboard_pointer)
  {
    wattron(win, COLOR_PAIR(KEY_SELECT_COLOR));
    waddnstr(win, "[DEL]", 5);
    wattroff(win, COLOR_PAIR(KEY_SELECT_COLOR));
  }
  else
  {
    waddnstr(win, "[DEL]", 5);
  }

  if(CONFIRM_CHAR_KEY == keyboard_pointer)
  {
    wattron(win, COLOR_PAIR(KEY_SELECT_COLOR));
    waddnstr(win, "[ENT]", 5);
    wattroff(win, COLOR_PAIR(KEY_SELECT_COLOR));
  }
  else
  {
    waddnstr(win, "[ENT]", 5);
  }
  return y;
}

static void w_print_continue(WINDOW* win, int y)
{
  const char button_string[] = "[ CONT ]";
  clear_line(win, y);

  const int x = GET_CENTER_IN_GAMEWINDOW(sizeof(button_string));
  wmove(win, y, x);
  wattron(win, COLOR_PAIR(KEY_SELECT_COLOR));
  waddnstr(win, button_string, sizeof(button_string));
  wattroff(win, COLOR_PAIR(KEY_SELECT_COLOR));
}

void render_game_over(View*       view,
                      const int   score,
                      const bool  draw_keyboard,
                      const char* name,
                      const char  keyboard_pointer)

{
  int next_line = render_string(view->game, 7, 1, game_over_str[0]);
  next_line     = render_string(view->game, 7, next_line, game_over_str[1]);

  w_print_number(view->game, 1, next_line, TETRIS_WIDTH * BLOCK_WIDTH, score);
  next_line += 5;

  if(draw_keyboard)
  {
    next_line = w_print_name(view->game, name, next_line);

    w_print_keyboard(view->game, next_line + 1, keyboard_pointer);
  }
  else
  {
    w_print_continue(view->game, next_line);
  }
}


static inline int create_rank(char* dest, const int place)
{
  switch(place)
  {
  case 1:
    memcpy(dest, "1st", sizeof(char) * 3);
    return 4;
    break;
  case 2:
    memcpy(dest, "2nd", sizeof(char) * 3);
    return 4;
    break;
  case 3:
    memcpy(dest, "3rd", sizeof(char) * 3);
    return 4;
    break;
  default:
    return snprintf(dest, 5, "%dth", place);
  }
}

void render_leader_board(View* view, const leaderboard_t* leaderboard, int y)
{
  const char title[]         = "HIGH SCORES";
  const char table_headers[] = "RANK    NAME    SCORE";

  clear_line(view->game, y);
  wmove(view->game, y, GET_CENTER_IN_GAMEWINDOW(sizeof(title)));
  waddnstr(view->game, title, sizeof(title));

  y += 2;

  int table_start = GET_CENTER_IN_GAMEWINDOW(sizeof(table_headers));

  clear_line(view->game, y);
  wmove(view->game, y, table_start);
  waddnstr(view->game, table_headers, sizeof(table_headers));

  y += 2;
  for(ssize_t s = leaderboard->score_count - 1; s >= 0; --s)
  {
    char score_buffer[TETRIS_WIDTH * BLOCK_WIDTH];
    memset(score_buffer, ' ', sizeof(score_buffer));

    const int end     = create_rank(score_buffer, leaderboard->score_count - s);
    score_buffer[end] = ' ';

    /* Aligned to "NAME" in table_headers */
    strncpy(score_buffer + 7, leaderboard->names[s], 3 * sizeof(char));

    /* Aligned to "SCORE" in table_headers */
    const int len = snprintf(score_buffer + 15,
                             sizeof(score_buffer) - 15,
                             "%d",
                             leaderboard->scores[s]);

    /* Move to start and print*/
    clear_line(view->game, y);
    wmove(view->game, y++, table_start);
    waddnstr(view->game, score_buffer, 15 + len);

    clear_line(view->game, y);
  }
}
