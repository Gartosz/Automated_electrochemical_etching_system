// Pin Definitions
#define DE_PIN 41
#define VSYNC_PIN 40
#define HSYNC_PIN 39
#define PCLK_PIN 42

#define R0_PIN 14
#define R1_PIN 21
#define R2_PIN 47
#define R3_PIN 48
#define R4_PIN 45

#define G0_PIN 9
#define G1_PIN 46
#define G2_PIN 3
#define G3_PIN 8
#define G4_PIN 16
#define G5_PIN 1

#define B0_PIN 15
#define B1_PIN 7
#define B2_PIN 6
#define B3_PIN 5
#define B4_PIN 4

// Sync Polarity and Timing
#define HSYNC_POLARITY 0
#define HSYNC_FRONT_PORCH 180
#define HSYNC_PULSE_WIDTH 30
#define HSYNC_BACK_PORCH 16

#define VSYNC_POLARITY 0
#define VSYNC_FRONT_PORCH 12
#define VSYNC_PULSE_WIDTH 13
#define VSYNC_BACK_PORCH 10

// Display Configuration
#define DISPLAY_WIDTH 800
#define DISPLAY_HEIGHT 480
#define DISPLAY_ROTATION 2
#define DISPLAY_AUTO_FLUSH true

// Touch Configuration
#define GFX_BL DF_GFX_BL
#define TFT_BL 2

#define TOUCH_GT911
#define TOUCH_GT911_SCL 20
#define TOUCH_GT911_SDA 19
#define TOUCH_GT911_INT -1
#define TOUCH_GT911_RST 38
#define TOUCH_GT911_ROTATION ROTATION_INVERTED
#define TOUCH_MAP_X1 800
#define TOUCH_MAP_X2 0
#define TOUCH_MAP_Y1 480
#define TOUCH_MAP_Y2 0
