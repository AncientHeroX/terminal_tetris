#include "sound.h"
#include <stdio.h>
#include <stdlib.h>

int init_sound(sound_ctl* sctl)
{
  ma_result result;
  result = ma_engine_init(NULL, &sctl->engine);
  if(result != MA_SUCCESS)
  {
    return -1;
  }
  return 0;
}


void destroy_sound(sound_ctl** sctl)
{
  ma_device_uninit(&(*sctl)->device);
  ma_decoder_uninit(&(*sctl)->decoder);
  ma_engine_uninit(&(*sctl)->engine);

  free(*sctl);
  *sctl = NULL;
}

static void data_callback(ma_device*  pDevice,
                          void*       pOutput,
                          const void* pInput,
                          ma_uint32   frameCount)
{
  ma_decoder* pDecoder = (ma_decoder*)pDevice->pUserData;
  if(pDecoder == NULL)
  {
    return;
  }

  /* Reading PCM frames will loop based on what we specified when called
   * ma_data_source_set_looping(). */
  ma_data_source_read_pcm_frames(pDecoder, pOutput, frameCount, NULL);

  (void)pInput;
}

static int decode_file(sound_ctl* engine, const char* filename)
{
  ma_result        result;
  ma_device_config deviceConfig;


  result = ma_decoder_init_file(filename, NULL, &engine->decoder);
  if(result != MA_SUCCESS)
  {
    return -2;
  }

  /*
  A engine->decoder is a data source which means we just use
  ma_data_source_set_looping() to set the looping state. We will read data using
  ma_data_source_read_pcm_frames() in the data callback.
  */
  ma_data_source_set_looping(&engine->decoder, MA_TRUE);

  deviceConfig                 = ma_device_config_init(ma_device_type_playback);
  deviceConfig.playback.format = engine->decoder.outputFormat;
  deviceConfig.playback.channels = engine->decoder.outputChannels;
  deviceConfig.sampleRate        = engine->decoder.outputSampleRate;
  deviceConfig.dataCallback      = data_callback;
  deviceConfig.pUserData         = &engine->decoder;

  if(ma_device_init(NULL, &deviceConfig, &engine->device) != MA_SUCCESS)
  {
    printf("Failed to open playback engine->device.\n");
    ma_decoder_uninit(&engine->decoder);
    return -3;
  }

  if(ma_device_start(&engine->device) != MA_SUCCESS)
  {
    printf("Failed to start playback engine->device.\n");
    ma_device_uninit(&engine->device);
    ma_decoder_uninit(&engine->decoder);
    return -4;
  }
  return 0;
}

static void start_main_theme(sound_ctl* sctl)
{
  decode_file(sctl, "resources/791018.wav");
}
void play_sound(sound_ctl* sctl, SOUND_TYPE type)
{
  switch(type)
  {
  case SOUND_MAIN_THEME:
    start_main_theme(sctl);
    return;
    break;
  case SOUND_LINE_CLEAR:
    ma_engine_play_sound(&sctl->engine, "resources/powerUp.wav", NULL);
    break;
  case SOUND_PLACE_BLOCK:
    ma_engine_play_sound(&sctl->engine, "resources/blipSelect.wav", NULL);
    break;
  }
}
