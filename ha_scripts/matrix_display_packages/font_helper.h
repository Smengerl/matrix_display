#pragma once

#include "esphome/components/font/font.h";


// Tracing tag
#define TAG "font_helper.h"


inline bool getFontBySelectId(int index, esphome::font::Font *font, int *deltaY) {
  switch (index) {
    case 0:
      *font = id(font_eight_bit_dragon);
      *deltaY = 0;
      break;
    case 1:
      *font = id(font_5by7);
      *deltaY = -1;
      break;
    case 2:
      *font = id(font_Coolville);
      *deltaY = +1;
      break;
    case 3:
      *font = id(font_editundo);
      *deltaY = 0;
      break;
    case 4:
      *font = id(font_game_over);
      *deltaY = +1;
      break;
    case 5:
      *font = id(font_GroutpixFTorch_Regular);
      *deltaY = +1;
      break;
    case 6:
      *font = id(font_ka1);
      *deltaY = -1;
      break;
    case 7:
      *font = id(font_Kenney_Rocket_Square);
      *deltaY = 0;
      break;
    case 8:
      *font = id(font_Kenney_Rocket);
      *deltaY = 0;
      break;
    case 9:
      *font = id(font_minikstt);
      *deltaY = -1;
      break;
    case 10:
      *font = id(font_Nihonium113);
      *deltaY = 0;
      break;
    case 11:
      *font = id(font_origa);
      *deltaY = 0;
      break;
    case 12:
      *font = id(font_PIXY);
      *deltaY = 0;
      break;
    case 13:
      *font = id(font_PXFXshadow_3);
      *deltaY = 0;
      break;
    case 14:
      *font = id(font_upheavtt);
      *deltaY = -1;
      break;
    case 15:
      *font = id(font_visitor1);
      *deltaY = 0;
      break;
    case 16:
      *font = id(font_Wendy_Neue);
      *deltaY = 0;
      break;
    default:
      return false;
  }
  return true;
}