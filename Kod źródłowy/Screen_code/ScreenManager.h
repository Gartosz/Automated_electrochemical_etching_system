#pragma once

#include <U8g2lib.h>
#include <Arduino_GFX_Library.h>
#include <TAMC_GT911.h>
#include "screen_defines.h"
#include "UserInterface.h"

class ScreenManager
{
  public:
  Arduino_RGB_Display *gfx = nullptr;

  ScreenManager(uint16_t _clickLockTime = 500);
  void init(TaskHandle_t *_taskToCall);
  void getCoordinates(int &touchX, int &touchY, uint64_t currentMs);
  bool canTouchBeProcessed(const uint64_t &currentMs);
  void checkTouch();

  private:
  Arduino_ESP32RGBPanel *rgbPanel_ = nullptr;
  TAMC_GT911 *ts_ = nullptr;
  TaskHandle_t *screenClickedTask_ = nullptr;
  uint64_t lastClickTime_ = 0;
  int lastTouchedXPos_ = 0;
  int lastTouchedYPos_ = 0;
  const uint16_t clickLockTime_;
};
