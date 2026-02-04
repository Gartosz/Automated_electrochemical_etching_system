#pragma once

#include <Arduino.h>
#include <Preferences.h>
#include "ScreenConnectionController.h"

#define CHART_WIDTH 604
#define CHART_HEIGHT 308
#define OPERATION_START (uint8_t)0x11
#define OPERATION_STOP (uint8_t)0x12
#define OPERATION_STANDBY (uint8_t)0x13
#define OPERATION_WAITING (uint8_t)0x14
#define MODE_VALUE (uint8_t)0x0A
#define VOLTAGE_VALUE (uint8_t)0x0B
#define CURRENT_VALUE (uint8_t)0x0C
#define FREQUENCY_VALUE (uint8_t)0x0D
#define WIRE_VALUE (uint8_t)0x0E
#define OPERATION_VALUE (uint8_t)0x0F
#define ACTIVATE_MOTOR_EXTRUDE (uint8_t)0x2A
#define ACTIVATE_MOTOR_RETRACT (uint8_t)0x2B
#define VPP_VALUE (uint8_t)0xD0
#define V_RMS_VALUE (uint8_t)0xD1
#define V_AVG_VALUE (uint8_t)0xD2
#define C_RMS_VALUE (uint8_t)0xD3
#define C_AVG_VALUE (uint8_t)0xD4

class ScreenDataHolder
{
public:
  enum class Modes : uint8_t {AC, DC, VCCS};
  enum class ExitCodes : uint8_t {Success, Timeout, CurrentOutOfRange, VoltageOutOfRange, CurrentDropped,
                        VoltageDropped, LoadDisconnected, Waiting};
  enum class DataValues : uint8_t {NoValue, Voltage, VoltageSign, Frequency, Current, Length};
  enum class Languages : uint8_t {Polish, English};
  const char* modesNames[3] = {"AC", "DC", "VCCS"};
  const char* codesMessages[8][2][2] = {
    {{"Zakończono pomyślnie.",  "Operation successful."},        {"", ""}},
    {{"Osiągnięto limit czasu.", "Timeout reached."},             {"", ""}},
    {{"Nie można osiągnąć",     "Cannot reach"},                  {"wybranego natężenia!", "the selected current!"}},
    {{"Nie można osiągnąć",     "Cannot reach"},                  {"wybranego napięcia!", "the selected voltage!"}},
    {{"Wykryto spadek natężenia.", "Current drop detected."},     {"Operacja zakończona.", "Operation complete."}},
    {{"Wykryto spadek napięcia.", "Voltage drop detected."},      {"Operacja zakończona.", "Operation complete."}},
    {{"Brak obciążenia",        "No load"},                       {"na wyjściu układu!", "on the output!"}},
    {{"Oczekiwanie na akcję.",  "Waiting for action."},           {"", ""}}
  };
  const char* languagesNames[2] = {"PL", "ENG"};
  const char *voltageParameterName = "voltage";
  const char *currentParameterName = "current";
  const char *frequencyParameterName = "frequency";
  const char *modeParameterName = "mode";
  const char *wireLengthParameterName = "length";
  const uint16_t bufferSize;

  Modes mode = Modes::AC;
  ExitCodes code = ExitCodes::Success;
  DataValues editedValue = DataValues::NoValue;
  Languages language = Languages::Polish;
  float vpp = 0.0f;
  float voltageRms = 0.0f;
  float voltageAvg = 0.0f;
  float currentRms = 0.0f;
  float currentAvg = 0.0f;
  float wireLength = 7.0f;
  float desiredVoltage = 1.0f;
  float desiredCurrent = 10.0f;
  uint16_t desiredFrequency = 100;
  uint8_t selectedButton = 0;
  int8_t switchButton = 0;
  char valueToSet[20] = "";
  char parameterName[20] = "";
  bool disconnected = false;
  bool operationNotDisplaying = true;
  bool editing = false;
  bool setParameter = true;
  bool operating = false;
  bool useButton = false;
  bool newData = false;
  bool drawData = false;

  ScreenDataHolder(unsigned int _bufferSize, ScreenConnectionController *_connection):
  bufferSize(_bufferSize)
  {
    connection_ = _connection;
  }
  
  int getMinVoltagePosition(size_t index);
  int getMaxVoltagePosition(size_t index);
  void incrementMode();
  void setValue();
  void setScreenColors(uint16_t _screenColor, uint16_t _textColor, uint16_t _backgroundTextColor);
  uint16_t getScreenColor();
  uint16_t getTextColor();
  uint16_t getBackgroundTextColor();
  uint8_t getTextSize();
  void setScreenColor(uint16_t color);
  void setTextColor(uint16_t color);
  void setBackgroundTextColor(uint16_t color);
  void setTextSize(uint8_t size);
  void updateData();
  void loadParameters();
  void sendAllSettings(const uint8_t &operation);
  void updateVoltageData(const uint16_t *const minVoltagesArray, const uint16_t *const maxVoltagesArray,
                         const uint16_t &minDataStartIndex, const uint16_t &maxDataStartIndex, const uint16_t &arraySize);
  void updateSignalData(const uint8_t &dataType, const float &value);

  const char* getModeCString()
  {
    return modesNames[static_cast<uint8_t>(mode)];
  }

  const char* getLanguageCString()
  {
    return languagesNames[static_cast<uint8_t>(language)];
  }

  const char* getCodeCString(size_t index = 0)
  {
    return codesMessages[static_cast<uint8_t>(code)][index][static_cast<uint8_t>(language)];
  }

  void sendExecutiveData(const ScreenConnectionController::DataPair* pairs, uint8_t count)
  {
    connection_->sendData(pairs, count);
  }

  void changeLanguage()
  {
    language = static_cast<Languages>((static_cast<uint8_t>(language) + 1) % 2);
  }

private:
  Preferences preferences_;
  ScreenConnectionController *connection_;
  uint16_t minVoltagePositions_[CHART_WIDTH] = {0};
  uint16_t maxVoltagePositions_[CHART_WIDTH] = {0};
  float voltageRange_[2] = {-5.0f, 5.0f};
  float currentRange_[2] = {0.0f, 100.0f};
  float lengthRange_[2] = {0.0f, 20.0f};
  int frequencyRange_[2] = {10, 1000};
  uint16_t screenColor_ = 0;
  uint16_t textColor_ = 0;
  uint16_t backgroundTextColor_ = 0;
  uint8_t textSize_ = 1;
  const char *preferencesNamespace_ = "parameters";

  void preferencesBegin(bool readonly);
  void preferencesEnd();
  void getSavedParameters();
  void saveParameter(const char *param_name, float value);
  void saveParameter(const char *param_name, uint8_t value);
  void saveParameter(const char *param_name, uint16_t value);
  void sendFloatValue(const char* paramName, const uint8_t &value_type, const float &value);
  void copyVoltage(uint16_t *destination, const uint16_t *const source, const uint16_t &startIndex, const uint16_t &arraySize);
};
