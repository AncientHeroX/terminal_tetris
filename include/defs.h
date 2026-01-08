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
  B_COLOR,
  LINE_CLEARED_COLOR,
  KEY_SELECT_COLOR
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

color_pairs get_type_color(block_type type);


#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

#define KEYBOARD_BUTTONS_PER_LINE 6
#define KEYBOARD_BUTTON_WIDTH 5
#define DEL_CHAR_KEY 'Z' + 1
#define CONFIRM_CHAR_KEY DEL_CHAR_KEY + 1
