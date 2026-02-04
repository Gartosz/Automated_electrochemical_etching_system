#pragma once

#include "ArduinoConnectionController.h"

class ControllersHandler;
class SignalManager;

#define OPERATION_START (uint8_t)0x11
#define OPERATION_STOP (uint8_t)0x12
#define OPERATION_STANDBY (uint8_t)0x13
#define OPERATION_WAITING (uint8_t)0x14
#define ENCODER_POSITION (uint8_t)0xB0
#define ENCODER_BUTTON (uint8_t)0xB1
#define MODE_VALUE (uint8_t)0x0A
#define VOTLAGE_VALUE (uint8_t)0x0B
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
#define CHART_WIDTH 604
#define CHART_HEIGHT 308
#define VERTICAL_PADDING_VOLTAGE 0.5f
#define MAX_THEORETICAL_CURRENT_READOUT 300

class DataHolder
{
public:
  enum class Modes {AC, DC, VCCS};

  Modes mode = Modes::AC;
  ControllersHandler *handler = nullptr;;
  SignalManager *manager = nullptr;
  uint16_t desiredFrequency = 100;
  int8_t switchButton = 0;
  float wireLength = 7.0f;
  float desiredVoltage = 1.0f;
  float desiredCurrent = 10.0f;
  bool operating = false;
  bool enableMotorForward = false;
  bool enableMotorBackward = false;
  bool useButton = false;
  bool waiting = false;
  constexpr static uint16_t voltageArraySize = 30000;
  constexpr static uint16_t currentArraySize = 5000;
  DataHolder(ControllersHandler *_handler, SignalManager *_manager) :
  handler(_handler), manager(_manager)
  {}

  void updateData(ArduinoConnectionController &connection);
  void sendMeasuredData(ArduinoConnectionController &connection);
  void updateVoltageArray(uint16_t voltage);
  void updateCurrentArray(float current);

  void setMode(uint8_t modeIndex) {
      mode = static_cast<Modes>(modeIndex % 3);
  }

private:
  const uint8_t disconnectedCountLimit_ = 100;
  uint16_t indexVoltage_ = 0;
  uint16_t indexCurrent_ = 0;
  float vpp_ = 0.0f;
  float voltageRms_ = 0.0f;
  float voltageAvg_ = 0.0f;
  float currentRms_ = 0.0f;
  float currentAvg_ = 0.0f;
  float voltageSumOfSquares_ = 0.0f;
  float currentSumOfSquares_ = 0.0f;
  float maxVoltage_ = 0.0f;
  float minVoltage_ = 0.0f;
  uint16_t minVoltagePositions_[CHART_WIDTH] = {0};
  uint16_t maxVoltagePositions_[CHART_WIDTH] = {0};
  uint16_t voltageArray_[voltageArraySize] = {0};
  float voltageRange_[2] = {-5.0f, 5.0f};
  float currentRange_[2] = {0.0f, 100.0f};
  float lengthRange_[2] = {0.0f, 20.0f};
  float currentArray_[currentArraySize] = {0.0f};
  int frequencyRange_[2] = {50, 1000};
  bool disconnected_ = false;
  void parseData(const uint8_t * const buffer, const uint8_t &size);
  void setOperation(uint8_t value);
  uint8_t setData(uint8_t dataType, uint8_t valueType, const uint8_t *value);
  float parseFloat(const uint8_t* value);
  void checkIfDisconnected(uint8_t &disconnectedCounter);
  void countDisconnectedTime(float previousCurrent, const float &readCurrent, uint8_t &disconnectedCounter);
  void updateCurrentData();
  uint16_t samplesToVerticalCord(const float &voltageValue);
  void updateCalculationsForSample(const float &voltageValue);
  void processSamplesForChart(uint16_t &valuesCount);
  void updateVoltageData();
};
