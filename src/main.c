#include <ncurses.h>
#include <stdlib.h>
#include <time.h>

#include "view.h"
#include "game_engine.h"

#define TARGET_FPS 60.0f

int main()
{
  View* game_view = (View*)malloc(sizeof(View));
  view_create(game_view);

  game_data* data = (game_data*)malloc(sizeof(game_data));
  init_game_state(data);
  system(
    "$(while [ 1 ]; do paplay /home/eduardglez/Downloads/791018.mp3; done) &");

  struct timespec start, now;
  clock_gettime(CLOCK_REALTIME, &start);
  while(1)
  {
    clock_gettime(CLOCK_REALTIME, &now);
    long delta_time_ms = (now.tv_sec - start.tv_sec) * 1000
                         + (now.tv_nsec - start.tv_nsec) / 1000000;

    update(data);

    if(delta_time_ms > 1000 / TARGET_FPS)
    {
      view_clear(game_view);
      draw(game_view, data);

      view_refresh(game_view);
    }
  }

  free(game_view);
  free(data);

  view_destroy(game_view);
  return 0;
}
