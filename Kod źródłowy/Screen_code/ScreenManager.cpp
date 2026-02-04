#include "ScreenManager.h"

ScreenManager::ScreenManager(uint16_t _clickLockTime) : clickLockTime_(_clickLockTime)
{
  #if defined(DISPLAY_DEV_KIT)
    gfx = create_default_Arduino_GFX();
  #else
    rgbPanel_ = new Arduino_ESP32RGBPanel(
    DE_PIN, VSYNC_PIN, HSYNC_PIN, PCLK_PIN,
    R0_PIN, R1_PIN, R2_PIN, R3_PIN, R4_PIN,
    G0_PIN, G1_PIN, G2_PIN, G3_PIN, G4_PIN, G5_PIN,
    B0_PIN, B1_PIN, B2_PIN, B3_PIN, B4_PIN,
    HSYNC_POLARITY, HSYNC_FRONT_PORCH, HSYNC_PULSE_WIDTH, HSYNC_BACK_PORCH,
    VSYNC_POLARITY, VSYNC_FRONT_PORCH, VSYNC_PULSE_WIDTH, VSYNC_BACK_PORCH
    );
    gfx = new Arduino_RGB_Display(
        DISPLAY_WIDTH, DISPLAY_HEIGHT, rgbPanel_, DISPLAY_ROTATION, DISPLAY_AUTO_FLUSH
    );
  #endif
  ts_ = new TAMC_GT911(TOUCH_GT911_SDA, TOUCH_GT911_SCL, TOUCH_GT911_INT, TOUCH_GT911_RST, max(TOUCH_MAP_X1, TOUCH_MAP_X2), max(TOUCH_MAP_Y1, TOUCH_MAP_Y2));
}

void ScreenManager::init(TaskHandle_t *_taskToCall)
{
  gfx->begin();
  gfx->setUTF8Print(true);
  gfx->setFont(u8g2_font_unifont_t_polish);
  gfx->fillScreen(RGB565_BLACK);

  #ifdef TFT_BL
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);
  #endif
   
  pinMode(TOUCH_GT911_RST, OUTPUT);
  digitalWrite(TOUCH_GT911_RST, LOW);
  delay(10);
  digitalWrite(TOUCH_GT911_RST, HIGH);
  delay(10);
  ts_->begin();
  ts_->setRotation(TOUCH_GT911_ROTATION);
  screenClickedTask_ = _taskToCall;
}

bool ScreenManager::canTouchBeProcessed(const uint64_t &currentMs)
{
  return (currentMs - lastClickTime_) > clickLockTime_;
}

void ScreenManager::checkTouch()
{
  while(true)
  {
    ts_->read();
    if (ts_->isTouched)
    {
      #if defined(TOUCH_SWAP_XY)
        lastTouchedXPos_ = map(ts_->points[0].y, TOUCH_MAP_X1, TOUCH_MAP_X2, 0, gfx->width() - 1);
        lastTouchedYPos_ = map(ts_->points[0].x, TOUCH_MAP_Y1, TOUCH_MAP_Y2, 0, gfx->height() - 1);
      #else
        lastTouchedXPos_ = map(ts_->points[0].x, TOUCH_MAP_X1, TOUCH_MAP_X2, 0, gfx->width() - 1);
        lastTouchedYPos_ = map(ts_->points[0].y, TOUCH_MAP_Y1, TOUCH_MAP_Y2, 0, gfx->height() - 1);
      #endif
        xTaskNotify(*screenClickedTask_, NULL, eSetValueWithOverwrite);
    }
    vTaskDelay(100);
  }
}

void ScreenManager::getCoordinates(int &touchX, int &touchY, uint64_t currentMs)
{
  touchX = lastTouchedXPos_;
  touchY = lastTouchedYPos_;
  lastClickTime_ = currentMs;
}