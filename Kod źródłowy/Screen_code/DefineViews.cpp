#include "DefineViews.h"

namespace ButtonsData
{
    namespace OperationView
    {
      View::StaticButtonData START = {10, 10, 160, 70, {"Start", "Start"}, RGB565_WHITE, RGB565_PURPLE};
      View::StaticButtonData STOP = {185, 10, 155, 70, {"Stop", "Stop"}, RGB565_WHITE, RGB565_PURPLE};
      View::StaticButtonData EXTRUDE = {355, 10, 210, 70, {"Wysuń drut", "Extrude wire"}, RGB565_WHITE, RGB565_PURPLE};
      View::StaticButtonData RETRACT = {580, 10, 210, 70, {"Schowaj drut", "Retract wire"}, RGB565_WHITE, RGB565_PURPLE};
      View::StaticButtonData ETCHING = {20, 405, 360, 70, {"Trawienie", "Etching"}, RGB565_WHITE, RGB565_PURPLE};
      View::StaticButtonData SETTINGS = {420, 405, 360, 70, {"Ustawienia", "Settings"}, RGB565_WHITE, RGB565_PURPLE};
    }

    namespace SettingsView
    {
      View::StaticButtonData SIGNAL_MODE = {200, 5, 400, 70, {"Tryb sygnału", "Signal mode"}, RGB565_WHITE, RGB565_PURPLE};
      View::StaticButtonData VOLTAGE = {200, 85, 320, 70, {"Napięcie", "Voltage"}, RGB565_WHITE, RGB565_PURPLE};
      View::StaticButtonData MINUS = {530, 85, 70, 70, {"-", "-"}, RGB565_WHITE, RGB565_PURPLE};
      View::StaticButtonData FREQUENCY = {200, 165, 400, 70, {"Częstotliwość", "Frequency"}, RGB565_WHITE, RGB565_PURPLE};
      View::StaticButtonData CURRENT = {200, 245, 400, 70, {"Natężenie", "Current"}, RGB565_WHITE, RGB565_PURPLE};
      View::StaticButtonData WIRE_LENGTH = {200, 325, 400, 70, {"Długość drutu", "Wire length"}, RGB565_WHITE, RGB565_PURPLE};
      View::StaticButtonData ETCHING = {10, 405, 330, 70, {"Trawienie", "Etching"}, RGB565_WHITE, RGB565_PURPLE};
      View::StaticButtonData EMPTY_CENTER = {350, 405, 100, 70, {"", ""}, RGB565_WHITE, RGB565_PURPLE};
      View::StaticButtonData SETTINGS = {460, 405, 330, 70, {"Ustawienia", "Settings"}, RGB565_WHITE, RGB565_PURPLE};
      View::StaticButtonData EMPTY_LEFT_1 = {40, 85, 100, 70, {"", ""}, RGB565_WHITE, RGB565_PURPLE};
      View::StaticButtonData EMPTY_LEFT_2 = {40, 245, 100, 70, {"", ""}, RGB565_WHITE, RGB565_PURPLE};
    }

    namespace NotificationView
    {
      View::StaticButtonData START = {100, 350, 250, 80, {"Uruchom", "Start"}, RGB565_GREEN, RGB565_BLACK};
      View::StaticButtonData BACK = {450, 350, 200, 80, {"Wróć", "Back"}, RGB565_GREEN, RGB565_BLACK};
    }

    namespace NumKeyboard
    {
      View::StaticButtonData NUM_0 = {165, 360, 100, 100, {"0", "0"}, RGB565_MAROON, RGB565_CYAN};
      View::StaticButtonData NUM_1 = {50, 245, 100, 100, {"1", "1"}, RGB565_MAROON, RGB565_CYAN};
      View::StaticButtonData NUM_2 = {165, 245, 100, 100, {"2", "2"}, RGB565_MAROON, RGB565_CYAN};
      View::StaticButtonData NUM_3 = {280, 245, 100, 100, {"3", "3"}, RGB565_MAROON, RGB565_CYAN};
      View::StaticButtonData NUM_4 = {50, 130, 100, 100, {"4", "4"}, RGB565_MAROON, RGB565_CYAN};
      View::StaticButtonData NUM_5 = {165, 130, 100, 100, {"5", "5"}, RGB565_MAROON, RGB565_CYAN};
      View::StaticButtonData NUM_6 = {280, 130, 100, 100, {"6", "6"}, RGB565_MAROON, RGB565_CYAN};
      View::StaticButtonData NUM_7 = {50, 15, 100, 100, {"7", "7"}, RGB565_MAROON, RGB565_CYAN};
      View::StaticButtonData NUM_8 = {165, 15, 100, 100, {"8", "8"}, RGB565_MAROON, RGB565_CYAN};
      View::StaticButtonData NUM_9 = {280, 15, 100, 100, {"9", "9"}, RGB565_MAROON, RGB565_CYAN};
    
      View::StaticButtonData DOT = {50, 360, 100, 100, {".", "."}, RGB565_MAROON, RGB565_CYAN};
      View::StaticButtonData CLEAR = {285, 360, 100, 100, {"C", "C"}, RGB565_MAROON, RGB565_CYAN};
      View::StaticButtonData SAVE = {480, 15, 250, 100, {"Zapisz", "Save"}, RGB565_NAVY, RGB565_ORANGE};
      View::StaticButtonData RETURN = {480, 125, 250, 100, {"Powrót", "Return"}, RGB565_NAVY, RGB565_ORANGE};

      View::StaticButtonData* NUMBERS[10] = {
        &NUM_0, &NUM_1, &NUM_2, &NUM_3, &NUM_4, 
        &NUM_5, &NUM_6, &NUM_7, &NUM_8, &NUM_9
      };
    }
}

View** makeViews(size_t &viewsCount)
{
  viewsCount = 4;
  View* operationView = createOperationView();
  View* settingsView = createSettingsView();
  View* numericalKeyboard = createNumericalKeyboard();
  View* notification = createNotificationView();
  operationView->setLinkedView(5, settingsView);
  settingsView->setLinkedView(1, numericalKeyboard);
  linkViews(3, 5, settingsView, numericalKeyboard, numericalKeyboard, numericalKeyboard, operationView, settingsView);
  numericalKeyboard->setLinkedView(12, settingsView);
  numericalKeyboard->setLinkedView(13, settingsView);
  notification->setLinkedView(0, operationView);
  notification->setLinkedView(1, operationView);
  View** views = new View*[viewsCount];
  views[0] = operationView;
  views[1] = settingsView;
  views[2] = numericalKeyboard;
  views[3] = notification;
  return views;
}

View* createOperationView()
{
  View *operationView = new View(6, 2, RGB565_BLACK);
    operationView->createButton(0, &ButtonsData::OperationView::START);
    operationView->createButton(1, &ButtonsData::OperationView::STOP);
    operationView->createButton(2, &ButtonsData::OperationView::EXTRUDE);
    operationView->createButton(3, &ButtonsData::OperationView::RETRACT);
    operationView->createButton(4, &ButtonsData::OperationView::ETCHING);
    operationView->createButton(5, &ButtonsData::OperationView::SETTINGS);
  
  operationView->setFunctionPointer(-1, &operationDataView);
  operationView->setFunctionPointer(0, &start);
  operationView->setFunctionPointer(1, &stop);
  operationView->setFunctionPointer(2, &motorExtrude);
  operationView->setFunctionPointer(3, &motorRetract);
  operationView->setFunctionPointer(5, &hideOperations);
  return operationView;
}

View* createSettingsView()
{
  View *settingsView = new View(11, 3, RGB565_BLACK);
  settingsView->createButton(0, &ButtonsData::SettingsView::SIGNAL_MODE);
  settingsView->createButton(1, &ButtonsData::SettingsView::VOLTAGE);
  settingsView->createButton(2, &ButtonsData::SettingsView::MINUS);
  settingsView->createButton(3, &ButtonsData::SettingsView::FREQUENCY);
  settingsView->createButton(4, &ButtonsData::SettingsView::CURRENT);
  settingsView->createButton(5, &ButtonsData::SettingsView::WIRE_LENGTH);
  settingsView->createButton(6, &ButtonsData::SettingsView::ETCHING);
  settingsView->createButton(7, &ButtonsData::SettingsView::EMPTY_CENTER);
  settingsView->createButton(8, &ButtonsData::SettingsView::SETTINGS);
  settingsView->createButton(9, &ButtonsData::SettingsView::EMPTY_LEFT_1);
  settingsView->createButton(10, &ButtonsData::SettingsView::EMPTY_LEFT_2);

  settingsView->setFunctionPointer(-1, &showSettingsData);
  settingsView->setFunctionPointer(0, &setOperationMode);
  settingsView->setFunctionPointer(1, &beginParameterEdit);
  settingsView->setFunctionPointer(2, &changeVoltageSign);
  settingsView->setFunctionPointer(3, &beginParameterEdit);
  settingsView->setFunctionPointer(4, &beginParameterEdit);
  settingsView->setFunctionPointer(5, &beginParameterEdit);
  settingsView->setFunctionPointer(7, &changeLanguage);
  settingsView->setFunctionPointer(9, &toggleVoltageValidation);
  settingsView->setFunctionPointer(10, &toggleCurrentValidation);
  return settingsView;
}

View* createNumericalKeyboard()
{
  View *numericalKeyboard = new View(14, 5, RGB565_BLACK);
  for (size_t index = 0; index < 10; ++index)
  {
    numericalKeyboard->createButton(index, ButtonsData::NumKeyboard::NUMBERS[index]);
    numericalKeyboard->setFunctionPointer(index, &getInput);
  }
    
  numericalKeyboard->createButton(10, &ButtonsData::NumKeyboard::DOT);
  numericalKeyboard->createButton(11, &ButtonsData::NumKeyboard::CLEAR);
  numericalKeyboard->createButton(12, &ButtonsData::NumKeyboard::SAVE);
  numericalKeyboard->createButton(13, &ButtonsData::NumKeyboard::RETURN);
  numericalKeyboard->setFunctionPointer(10, &getInput);
  numericalKeyboard->setFunctionPointer(11, &getInput);
  numericalKeyboard->setFunctionPointer(12, &setData);
  numericalKeyboard->setFunctionPointer(13, &resetEdit);
  numericalKeyboard->setFunctionPointer(-1, &editingParameterView);
  return numericalKeyboard;
}

View* createNotificationView()
{
  View *notificationView = new View(2, 4, RGB565_RED);
  notificationView->createButton(0, &ButtonsData::NotificationView::START);
  notificationView->createButton(1, &ButtonsData::NotificationView::BACK);
  notificationView->setFunctionPointer(0, &startAgain);
  notificationView->setFunctionPointer(1, &standbyMode);
  notificationView->setFunctionPointer(-1, &setAlert);
  return notificationView;
}

void linkViews(size_t startIndex, size_t numViews, ...) {
  int buttonIndex = startIndex;

  va_list args;
  va_start(args, numViews);

  View* baseView = va_arg(args, View*);
  View* nextView;

  for (size_t i = 1; i <= numViews; ++i)
  {
    nextView = va_arg(args, View*);
    baseView->setLinkedView(buttonIndex++, nextView);
  }

  va_end(args);
}

void operationDataView(ScreenDataHolder *data, Arduino_RGB_Display *gfx)
{
  static const uint16_t bgColor = data->getBackgroundTextColor();
  static char buffer[10];
  if (data->operationNotDisplaying)
  {
    gfx->fillRect(10, 90, 150, 300, bgColor);
    gfx->setCursor(25, 130);
    gfx->print("<I RMS>");
    gfx->setCursor(25, 230);
    gfx->print("<V RMS>");
    gfx->setCursor(45, 330);
    gfx->print("<Vpp>");
    gfx->setCursor(127, 170);
    gfx->print("mA");
    gfx->setCursor(140, 270);
    gfx->print("V");
    gfx->setCursor(140, 370);
    gfx->print("V");
    drawChart(gfx, data);
    data->operationNotDisplaying = false;
    data->code = ScreenDataHolder::ExitCodes::Success;
    data->newData = true;
  }

  if(data->newData)
  {
    gfx->setTextColor(RGB565_WHITE, RGB565_PURPLE);
    data->setTextColor(RGB565_WHITE);
    data->setBackgroundTextColor(RGB565_PURPLE);
    gfx->fillRect(10, 140, 116, 30, RGB565_PURPLE);
    gfx->setCursor(15, 170);
    snprintf(buffer, sizeof(buffer), "%.2f", static_cast<uint8_t>(data->mode) ? data->currentAvg : data->currentRms);
    gfx->print(buffer);
    gfx->fillRect(10, 250, 119, 30, RGB565_PURPLE);
    gfx->setCursor(15, 270);
    snprintf(buffer, sizeof(buffer), "%.2f", data->voltageRms);
    gfx->print(buffer);
    gfx->fillRect(10, 350, 119, 30, RGB565_PURPLE);
    gfx->setCursor(15, 370);
    snprintf(buffer, sizeof(buffer), "%.2f", data->vpp);
    gfx->print(buffer);
    data->newData = false;
  }
  if(data->drawData)
  {
    drawChartData(gfx, data, 191, 91, CHART_WIDTH, CHART_HEIGHT);
    data->drawData = false;
  }
}

void drawChart(Arduino_RGB_Display *gfx, ScreenDataHolder *data)
{
  gfx->fillRect(190, 90, CHART_WIDTH + 2, CHART_HEIGHT + 2, RGB565_BLACK);
  gfx->drawRect(190, 90,  CHART_WIDTH + 2, CHART_HEIGHT + 2, RGB565_GREEN);
  gfx->drawFastHLine(191, 245, CHART_WIDTH, RGB565_INDIANRED);
  gfx->setTextColor(data->getTextColor(), RGB565_BLACK);
  gfx->setTextSize(1);
  for (int i = 5; i >= -5; --i)
  {
    gfx->setCursor(165, 250 - i * 27);
    gfx->print(i);
  }
  gfx->setTextSize(data->getTextSize());
  gfx->setTextColor(data->getTextColor(), data->getBackgroundTextColor());
}

void drawChartData(Arduino_RGB_Display *gfx, ScreenDataHolder *data, int xOffset, int yOffset, int length, int height)
{
  uint16_t currentMin, currentMax, nextMin, nextMax;
  gfx->fillRect(xOffset, yOffset, length, height, RGB565_BLACK);
  for (size_t index = 0; index < length; ++index)
  {
    currentMin = data->getMinVoltagePosition(index);
    currentMax = data->getMaxVoltagePosition(index);
    gfx->drawLine(xOffset + index, yOffset + currentMin, xOffset + index, yOffset + currentMax, RGB565_DARKGREY);
    if (index < length - 1)
    {
      nextMin = data->getMinVoltagePosition(index + 1);
      nextMax = data->getMaxVoltagePosition(index + 1);
      float trend = (currentMin + currentMax) / 2 - (nextMin + nextMax) / 2;
      gfx->drawLine(xOffset + index, yOffset + (trend > 0 ? currentMax: currentMin), xOffset + index + 1, yOffset + (trend > 0 ? nextMin: nextMax), RGB565_DARKGREY);
    }
  }
  gfx->drawFastHLine(xOffset, yOffset + height / 2, length, RGB565_INDIANRED);
}

void printValidationMark(Arduino_RGB_Display *gfx, const bool &enabled, const int &x, const int &y, const int &width, const int &height, const uint8_t &textSize)
{
  static const char validationEnabledSign[8] = "✔";
  static const char validationDisabledSign[8] = "✘";
  uint8_t symbolWidth = 16 * textSize;
  uint8_t symbolHeight = 12 * textSize;
  const char* sign = enabled ? validationEnabledSign : validationDisabledSign;
  gfx->setTextSize(textSize);
  gfx->setFont(u8g2_font_unifont_t_symbols);
  gfx->setCursor(x + (width - symbolWidth) / 2, y + (height + symbolHeight) / 2);
  gfx->print(sign);
  gfx->setFont(u8g2_font_unifont_t_polish);
}

void printSelectedLanguage(Arduino_RGB_Display *gfx, const char* text, const uint16_t color, const int &x, const int &y, const int &width, const int &height, const uint8_t &textSize)
{
  gfx->setTextSize(textSize);
  gfx->fillRect(x, y, width, height, color);
  int textBegin[2];
  View::getTextAlignment(gfx, x, y, width, height, text, textBegin[0], textBegin[1]);
  gfx->setCursor(textBegin[0], textBegin[1]);
  gfx->print(text);
}

void setColor(Arduino_RGB_Display *gfx, ScreenDataHolder *data, uint8_t index)
{
  if (index == data->selectedButton)
    gfx->setTextColor(SELECTED_TEXT_COLOR, SELECTED_BUTTON_COLOR);
  else
    gfx->setTextColor(data->getTextColor(), data->getBackgroundTextColor());
}

void showSettingsData(ScreenDataHolder *data, Arduino_RGB_Display *gfx)
{
  char buffer[10];
  gfx->setTextColor(RGB565_YELLOW, RGB565_BLACK);
  gfx->setCursor(650, 55);
  gfx->print(data->getModeCString());
  gfx->setCursor(650, 135);
  snprintf(buffer, sizeof(buffer), "%.2f", data->desiredVoltage);
  gfx->print(buffer);
  gfx->setCursor(650, 215);
  snprintf(buffer, sizeof(buffer), "%u", data->desiredFrequency);
  gfx->print(buffer);
  gfx->setCursor(650, 300);
  snprintf(buffer, sizeof(buffer), "%.2f", data->desiredCurrent);
  gfx->print(buffer);
  gfx->setCursor(650, 380);
  snprintf(buffer, sizeof(buffer), "%.2f", data->wireLength);
  gfx->print(buffer);
  gfx->setTextSize(2);
  gfx->setCursor(35, 30);
  gfx->print((data->language == ScreenDataHolder::Languages::Polish) ? "Kontrola" : "Parameters");
  gfx->setCursor(15, 70);
  gfx->print((data->language == ScreenDataHolder::Languages::Polish) ? "parametrów:" : "control:");

  setColor(gfx, data, 7);
  printSelectedLanguage(gfx, data->getLanguageCString(), data->getBackgroundTextColor(), 350, 405, 100, 70);
  setColor(gfx, data, 8);
  printValidationMark(gfx, data->validateVoltage, 40, 85, 100, 70);
  setColor(gfx, data, 9);
  printValidationMark(gfx, data->validateCurrent, 40, 245, 100, 70);
  gfx->setTextSize(data->getTextSize());
}

void setOperationMode(ScreenDataHolder *data, Arduino_RGB_Display *gfx)
{
  data->incrementMode();
  gfx->fillRect(620, 5, 180, 70, RGB565_BLACK);
}

void beginParameterEdit(ScreenDataHolder *data, Arduino_RGB_Display *gfx __attribute__((unused)), const char *name, size_t index)
{
  data->editedValue = static_cast<ScreenDataHolder::DataValues>(index);
  strcpy(data->parameterName, name);
}

void getInput(ScreenDataHolder *data, Arduino_RGB_Display *gfx, const char *value)
{
  if (strcmp(value, "C") == 0)
    data->valueToSet[0] = '\0';
  else if (strlen(data->valueToSet) < 7)
    strncat(data->valueToSet, value, sizeof(data->valueToSet) - strlen(data->valueToSet) - 1);
  gfx->fillRoundRect(450, 350, 300, 70, 8, RGB565_DARKGREY);
  gfx->setTextColor(RGB565_BLUE, RGB565_DARKGREY);
  data->setScreenColors(RGB565_DARKGREY, RGB565_BLUE, RGB565_DARKGREY);
  gfx->setCursor(460, 410);
  gfx->print(data->valueToSet);
}

void setData(ScreenDataHolder *data)
{
  data->setValue();
  resetEdit(data);
}

void resetEdit(ScreenDataHolder *data)
{
  data->editedValue = ScreenDataHolder::DataValues::NoValue;
  data->valueToSet[0] = '\0';
  data->editing = false;
  data->parameterName[0] = '\0';
}

void editingParameterView(ScreenDataHolder *data, Arduino_RGB_Display *gfx)
{
  if (data->editing)
    return;
  data->editing = true;
  gfx->setTextColor(RGB565_GREEN, RGB565_BLACK);
  gfx->setTextSize(3);
  gfx->setCursor(450, 300);
  gfx->print(data->parameterName);
  gfx->setTextSize(data->getTextSize());
  gfx->fillRoundRect(450, 350, 300, 70, 8, RGB565_DARKGREY);
  data->setScreenColors(RGB565_DARKGREY, RGB565_GREEN, RGB565_BLACK);
}

void start(ScreenDataHolder *data)
{
  data->operating = true;
  data->sendAllSettings(OPERATION_START);
}

void stop(ScreenDataHolder *data)
{
  data->operating = false;
  data->sendExecutiveData((ScreenConnectionController::DataPair[]){
      ScreenConnectionController::DataPair(OPERATION_VALUE, OPERATION_STOP)
  }, 1);
}

void hideOperations(ScreenDataHolder *data)
{
  data->operationNotDisplaying = true;
}

void motorExtrude(ScreenDataHolder *data)
{
  if (!(data->operating))
    data->sendExecutiveData((ScreenConnectionController::DataPair[]){
      ScreenConnectionController::DataPair(ACTIVATE_MOTOR_EXTRUDE, (uint8_t)0)
    }, 1);
}

void motorRetract(ScreenDataHolder *data)
{
  if (!(data->operating))
    data->sendExecutiveData((ScreenConnectionController::DataPair[]){
      ScreenConnectionController::DataPair(ACTIVATE_MOTOR_RETRACT, (uint8_t)0)
    }, 1);
}

void changeVoltageSign(ScreenDataHolder *data, Arduino_RGB_Display *gfx)
{
  data->editedValue = static_cast<ScreenDataHolder::DataValues>(2);
  data->setValue();
  resetEdit(data);
  gfx->fillRect(620, 85, 180, 70, RGB565_BLACK);
}

void startAgain(ScreenDataHolder *data)
{
  data->editing = false;
  start(data);
}

void standbyMode(ScreenDataHolder *data)
{
  data->sendExecutiveData((ScreenConnectionController::DataPair[]){
    ScreenConnectionController::DataPair(OPERATION_VALUE, OPERATION_STANDBY)
  }, 1);
}

void setAlert(ScreenDataHolder *data, Arduino_RGB_Display *gfx)
{
  if(data->code != ScreenDataHolder::ExitCodes::Waiting)
  {
    hideOperations(data);
    data->sendExecutiveData((ScreenConnectionController::DataPair[]){
      ScreenConnectionController::DataPair(OPERATION_VALUE, OPERATION_WAITING)
    }, 1);
    gfx->fillRect(20, 20, 760, 300, RGB565_YELLOW);
    gfx->setTextSize(3);
    gfx->setTextColor(RGB565_BLACK, RGB565_YELLOW);
    for (size_t i = 0; i < 2; ++i)
    {
      const char* text = data->getCodeCString(i);
      if (!text)
        break;
      int textBegin[2];
      View::getTextAlignment(gfx, 20, 20 + 150 * i, 760, 150, text, textBegin[0], textBegin[1]);
      gfx->setCursor(textBegin[0], textBegin[1]);
      gfx->print(text);
    }
    gfx->setTextSize(data->getTextSize());
    gfx->setTextColor(data->getTextColor(), data->getBackgroundTextColor());
    data->code = ScreenDataHolder::ExitCodes::Waiting;
  }
}

void toggleVoltageValidation(ScreenDataHolder *data, Arduino_RGB_Display *gfx)
{
  data->validateVoltage = !data->validateVoltage;
  gfx->fillRect(40, 85, 100, 70, RGB565_PURPLE);
}

void toggleCurrentValidation(ScreenDataHolder *data, Arduino_RGB_Display *gfx)
{
  data->validateCurrent = !data->validateCurrent;
  gfx->fillRect(40, 245, 100, 70, RGB565_PURPLE);
}

void changeLanguage(ScreenDataHolder *data, Arduino_RGB_Display *gfx)
{
  data->changeLanguage();
}
