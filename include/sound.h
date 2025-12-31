#pragma once
#include "miniaudio.h"

typedef struct
{
  ma_engine  engine;
  ma_decoder decoder;
  ma_device  device;
} sound_ctl;

typedef enum
{
  SOUND_MAIN_THEME,
  SOUND_LINE_CLEAR,
  SOUND_PLACE_BLOCK
} SOUND_TYPE;

int  init_sound(sound_ctl* engine);
void destroy_sound(sound_ctl** sctl);

void play_sound(sound_ctl* sctl, SOUND_TYPE type);
