#include <stdio.h>
#include <ncurses.h>

int main()
{

  initscr();
  cbreak();
  noecho();
  curs_set(0);

  addstr("Hello, world");
  refresh();
  while(1)
  {
  }

  endwin();
  return 0;
}
