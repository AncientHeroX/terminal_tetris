#include <ncurses.h>
#include <stdlib.h>
#include <time.h>

#include "sound.h"
#include "view.h"
#include "game_engine.h"

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

#define TARGET_FPS 60.0f
static const long frame_time_us = 1000000 / TARGET_FPS;

int main()
{
  View* game_view = (View*)malloc(sizeof(View));
  view_create(game_view);

  game_data* data = (game_data*)malloc(sizeof(game_data));
  init_game_state(data);

  sound_ctl* game_sound = (sound_ctl*)malloc(sizeof(sound_ctl));
  init_sound(game_sound);

  play_sound(game_sound, SOUND_MAIN_THEME);

  struct timespec start, now;

  clock_gettime(CLOCK_REALTIME, &start);
  while(1)
  {
    clock_gettime(CLOCK_REALTIME, &now);
    long delta_time_us = (now.tv_sec - start.tv_sec) * 1000000LL
                         + (now.tv_nsec - start.tv_nsec) / 1000LL;

    update(data, game_sound, delta_time_us);

    view_clear(game_view);

    draw(game_view, data);

    view_refresh(game_view);
    start = now;

    long sleep_time = delta_time_us - frame_time_us;
    if(sleep_time > 0)
    {
      usleep(sleep_time);
    }
  }

  free(game_view);
  free(data);

  destroy_sound(&game_sound);
  view_destroy(game_view);
  return 0;
}
