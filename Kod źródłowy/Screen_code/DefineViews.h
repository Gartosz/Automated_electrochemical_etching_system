#pragma once

#include "View.h"

View** makeViews(size_t &viewsCount);
View* createMainMenu();
View* createOperationView();
View* createSettingsView();
View* createNumericalKeyboard();
View* createNotificationView();
uint16_t determineButtonColor(uint8_t index);
void linkViews(size_t startIndex, size_t numViews, ...);
void operationDataView(ScreenDataHolder *data, Arduino_RGB_Display *gfx);
void drawChart(Arduino_RGB_Display *gfx, ScreenDataHolder *data);
void drawChartData(Arduino_RGB_Display *gfx, ScreenDataHolder *data,
                   int xOffset, int yOffset, int length, int height);
void showSettingsData(ScreenDataHolder *data, Arduino_RGB_Display *gfx);
void setOperationMode(ScreenDataHolder *data, Arduino_RGB_Display *gfx);
void beginParameterEdit(ScreenDataHolder *data, Arduino_RGB_Display *gfx __attribute__((unused)),
                        const char *name, size_t index);
void getInput(ScreenDataHolder *data, Arduino_RGB_Display *gfx, const char *value);
void setData(ScreenDataHolder *data);
void resetEdit(ScreenDataHolder *data);
void editingParameterView(ScreenDataHolder *data, Arduino_RGB_Display *gfx);
void start(ScreenDataHolder *data);
void stop(ScreenDataHolder *data);
void hideOperations(ScreenDataHolder *data);
void motorExtrude(ScreenDataHolder *data);
void motorRetract(ScreenDataHolder *data);
void changeVoltageSign(ScreenDataHolder *data, Arduino_RGB_Display *gfx);
void startAgain(ScreenDataHolder *data);
void standbyMode(ScreenDataHolder *data);
void setAlert(ScreenDataHolder *data, Arduino_RGB_Display *gfx);
void printValidationMark(Arduino_RGB_Display *gfx, const bool &enabled, const int &x, const int &y, const int &width, const int &height, const uint8_t &textSize = 4);
void printSelectedLanguage(Arduino_RGB_Display *gfx, const char* text, const uint16_t color, const int &x, const int &y, const int &width, const int &height, const uint8_t &textSize = 3);
void toggleVoltageValidation(ScreenDataHolder *data, Arduino_RGB_Display *gfx);
void toggleCurrentValidation(ScreenDataHolder *data, Arduino_RGB_Display *gfx);
void changeLanguage(ScreenDataHolder *data, Arduino_RGB_Display *gfx);
