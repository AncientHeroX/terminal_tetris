#pragma once

#include <stddef.h>

typedef char player_name[4];

#define COPY_NAME(d, s)                                                        \
  do                                                                           \
  {                                                                            \
    memcpy(d, s, sizeof(char) * 4);                                            \
  }                                                                            \
  while(0);

#define LEADERBOARD_CAP 10
typedef struct
{
  int         scores[LEADERBOARD_CAP];
  player_name names[LEADERBOARD_CAP];
  size_t      score_count;
} leaderboard_t;

typedef unsigned char byte_t;

int  load_leaderboard(leaderboard_t* leaderboard_buff);
void save_score(leaderboard_t     leaderboard,
                const player_name name,
                const int         score);
