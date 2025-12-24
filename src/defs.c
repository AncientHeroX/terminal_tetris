#include "defs.h"

color_pairs get_type_color(block_type type)
{
    switch(type)
  {
    case J:
      return J_COLOR;
      break;
    case L:
      return L_COLOR;
      break;
    case T:
      return T_COLOR;
      break;
    case Z:
      return Z_COLOR;
      break;
    case S:
      return S_COLOR;
      break;
    case I:
      return I_COLOR;
      break;
    case B:
      return B_COLOR;
      break;
    default:
      return B_COLOR;
      break;
  }
}