#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include "view.h"

#define TARGET_FPS 60.0f

int main()
{
  View* game_view = (View*)malloc(sizeof(View));
  view_create(game_view);

  float y = 0;


  struct timespec start, now;
  clock_gettime(CLOCK_REALTIME, &start);
  while(1)
  {
    clock_gettime(CLOCK_REALTIME, &now);
    long delta_time_ms = (now.tv_sec - start.tv_sec) * 1000
                         + (now.tv_nsec - start.tv_nsec) / 1000000;

    if(delta_time_ms > 1000 / TARGET_FPS)
    {
      view_clear(game_view);

      // DRAW GAME
      place_block(game_view, 5, y);
      y += 0.0002f;

      wborder(game_view->game,
              ACS_VLINE,
              ACS_VLINE,
              ACS_HLINE,
              ACS_HLINE,
              ACS_ULCORNER,
              ACS_URCORNER,
              ACS_LLCORNER,
              ACS_LRCORNER);

      wrefresh(game_view->game);
    }
  }

  view_destroy(game_view);
  return 0;
}
