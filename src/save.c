#include "save.h"
#include "debug.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

static void insertion_sort_scores(player_name* restrict names,
                                  int* restrict scores,
                                  const size_t count)
{
  for(size_t i = 1; i < count; i++)
  {
    int         curr_score = scores[i];
    player_name curr_name;
    COPY_NAME(curr_name, names[i]);

    size_t j = i - 1;
    while(j >= 0 && scores[j] > curr_score)
    {
      scores[j + 1] = scores[j];
      COPY_NAME(names[j + 1], names[j]);
      j--;
    }

    scores[j + 1] = curr_score;
    COPY_NAME(names[j + 1], curr_name);
  }
}


static void sort_scores(player_name* restrict names,
                        int* restrict scores,
                        const size_t count)
{
  return insertion_sort_scores(names, scores, count);
}

#define SAVE_DATA(fileptr, dataptr, s, n)                                      \
  if(fwrite(dataptr, s, n, fileptr) != n)                                      \
  {                                                                            \
    printf("Could not write %s\n", #dataptr);                                  \
    fclose(fileptr);                                                           \
    return;                                                                    \
  }

void save_score(leaderboard_t     leaderboard,
                const player_name name,
                const int         score)
{
  const char* leaderboard_dir = "leaderboard";

  struct stat st = { 0 };
  if(stat(leaderboard_dir, &st) == -1)
  {
    mkdir(leaderboard_dir, 0755);
  }

  FILE* save_file = fopen("leaderboard/scores", "w");

  if(leaderboard.score_count < LEADERBOARD_CAP)
  {
    leaderboard.scores[leaderboard.score_count] = score;
    COPY_NAME(leaderboard.names[leaderboard.score_count], name);

    leaderboard.score_count++;
  }
  else if(leaderboard.score_count == LEADERBOARD_CAP)
  {
    assert(score > leaderboard.scores[leaderboard.score_count - 1]);

    leaderboard.scores[0] = score;
    COPY_NAME(leaderboard.names[0], name);
  }
  else
  {
    assert(UNREACHABLE);
  }

  sort_scores(leaderboard.names, leaderboard.scores, leaderboard.score_count);

  if(ftell(save_file) == 0)
  {
    const char* signature = "LEAD";
    if(save_file)
      SAVE_DATA(save_file, signature, sizeof(char), 4);
  }

  for(size_t i = 0; i < leaderboard.score_count; i++)
  {
    dlog("saving %.4s - %d", leaderboard.names[i], leaderboard.scores[i]);

    SAVE_DATA(save_file, &leaderboard.names[i], sizeof(char), 4);
    SAVE_DATA(save_file, &leaderboard.scores[i], sizeof(int), 1);
  }

  fclose(save_file);
  return;
}

#undef SAVE_DATA

#define READ_DATA(fileptr, buffer, s, n)                                       \
  if(fread(buffer, s, n, fileptr) != n)                                        \
  {                                                                            \
    dlog("Failed to read at line\n");                                          \
    fclose(fileptr);                                                           \
    return 1;                                                                  \
  }


int load_leaderboard(leaderboard_t* leaderboard_buff)
{
  FILE* load_leaderboard = fopen("leaderboard/scores", "r+");

  if(load_leaderboard == NULL)
    return 1;

  const char expected_signature[4] = "LEAD";
  char       signature[4];

  READ_DATA(load_leaderboard, signature, sizeof(char), 4);
  for(int c = 0; c < 4; c++)
  {
    if(expected_signature[c] != signature[c])
    {
      dlog("Save file format incorrect\n");
      fclose(load_leaderboard);
      return 1;
    }
  }

  const long curr = ftell(load_leaderboard);
  fseek(load_leaderboard, 0, SEEK_END);
  fseek(load_leaderboard, curr, SEEK_SET);

  byte_t buffer[8];
  size_t score_index = 0;
  while(fread(buffer, sizeof(byte_t), 8, load_leaderboard)
          == (8 * sizeof(byte_t))
        && score_index < LEADERBOARD_CAP)
  {
    memcpy(leaderboard_buff->names + score_index, buffer, (sizeof(char) * 4));
    memcpy(leaderboard_buff->scores + score_index,
           buffer + 4,
           (sizeof(char) * 4));
    score_index++;
  }

  leaderboard_buff->score_count = score_index;
  dlog("loaded %zu scores\n", leaderboard_buff->score_count);

  fclose(load_leaderboard);
  return 0;
}
#undef READ_DATA
