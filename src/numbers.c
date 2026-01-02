#include "numbers.h"

#include <assert.h>
#include <ncurses.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static const int pow10_int[]
  = { 1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000 };

static const char* numbers[]
  = { "  __\n /  \\\n| () |\n \\__/\0", " _\n/ |\n| |\n|_|\0",
      " ___\n|_  )\n / /\n/___|\0",     " ____\n|__ /\n |_ \\\n|___/\0",
      " _ _\n| | |\n|_  _|\n  |_|\0",   " ___\n| __|\n|__ \\\n|___/\0",
      "  __\n / /\n/ _ \\\n\\___/\0",   " ____\n|__  |\n  / /\n /_/\0",
      " ___\n( _ )\n/ _ \\\n\\___/\0",  " ___\n/ _ \\\n\\_, /\n /_/\0" };
static const int widths[] = { 5, 3, 4, 4, 5, 4, 4, 5, 4, 4 };
#define NUMBER_HEIGHT 4
#define BUFF_WIDTH (9 * 5)


static void write_digit_to_buff(char buff[NUMBER_HEIGHT][BUFF_WIDTH + 1],
                                const size_t draw_pos,
                                const int    digit)
{
  if(digit < 0 || digit > 9)
    return;

  uint8_t ptr = 0;

  uint32_t x_offset = 0, y_offset = 0;

  char   c;
  size_t x;
  while((c = numbers[digit][ptr++]) != '\0')
  {
    assert(digit >= 0 && digit <= 9);
    if(c == '\n')
    {
      y_offset++;
      x_offset = 0;
      continue;
    }

    x = draw_pos + x_offset;
    x = x < BUFF_WIDTH ? x : BUFF_WIDTH;

    if(buff[y_offset][x] == ' ')
      buff[y_offset][x] = c;

    x_offset++;
  }
}


void w_print_number(WINDOW*   w,
                    const int x,
                    const int y,
                    const int width,
                    int       num)
{
  char buff[NUMBER_HEIGHT][BUFF_WIDTH + 1];
  for(size_t h = 0; h < NUMBER_HEIGHT; h++)
  {
    memset(buff[h], ' ', sizeof(buff[0]));
    buff[h][BUFF_WIDTH] = '\0';
  }

  int8_t curr_place = 7;

  int draw_pos = 0;

  while(curr_place >= 0)
  {
    int magnitude = pow10_int[curr_place];

    int curr_digit;
    if(num < magnitude)
      curr_digit = 0;
    else
      curr_digit = (int)(num / magnitude);

    write_digit_to_buff(buff, draw_pos, curr_digit);

    draw_pos += widths[curr_digit];

    if(draw_pos > BUFF_WIDTH)
      break;

    num -= pow10_int[curr_place] * curr_digit;
    curr_place--;
  }

  int middle = (int)(width / 2) - (int)(draw_pos / 2);

  for(int r = 0; r < NUMBER_HEIGHT; r++)
  {
    mvwaddstr(w, y + r, x + middle, buff[r]);
  }
}
