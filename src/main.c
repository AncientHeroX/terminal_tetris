#include <stdio.h>
#include <stdlib.h>
#include "view.h"

int main()
{
  View* game_view = (View*)malloc(sizeof(View));

  view_create(game_view);

  while(1)
  {
  }

  view_destroy(game_view);
  return 0;
}
