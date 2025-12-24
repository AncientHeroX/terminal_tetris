#pragma once
typedef enum
{
  J = 17600,
  L = 17504,
  T = 19968,
  Z = 50688,
  S = 27648,
  I = 34952,
  B = 52224
} block_type;

typedef enum
{
  J_COLOR = 1,
  L_COLOR,
  T_COLOR,
  Z_COLOR,
  S_COLOR,
  I_COLOR,
  B_COLOR
} color_pairs;

typedef enum
{
  UP,
  DOWN,
  LEFT,
  RIGHT
} rotation;

typedef struct
{
  float x, y;
} vector2;
