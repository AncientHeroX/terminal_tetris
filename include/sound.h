#pragma once
#include "miniaudio.h"

typedef struct
{
  ma_engine  engine;
  ma_decoder decoder;
  ma_device  device;
} sound_ctl;

int  init_sound(sound_ctl* engine);
void destroy_sound(sound_ctl** sctl);

void start_main_theme(sound_ctl* engine);
void play_place_sound(sound_ctl* sctl);
void play_clear_sound(sound_ctl* sctl);
