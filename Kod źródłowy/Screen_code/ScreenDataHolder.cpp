#include "ScreenDataHolder.h"

void ScreenDataHolder::updateVoltageData(const uint16_t *const minVoltagesArray, const uint16_t *const maxVoltagesArray, const uint16_t &minDataStartIndex, const uint16_t &maxDataStartIndex, const uint16_t &arraySize)
{
  copyVoltage(minVoltagePositions_, minVoltagesArray, minDataStartIndex, arraySize);
  copyVoltage(maxVoltagePositions_, maxVoltagesArray, maxDataStartIndex, arraySize);
  drawData = true;
}

void ScreenDataHolder::copyVoltage(uint16_t *destination, const uint16_t *const source, const uint16_t &startIndex, const uint16_t &arraySize)
{
  uint16_t firstChunk = min(CHART_WIDTH, arraySize - startIndex);
  uint16_t secondChunk = CHART_WIDTH - firstChunk;
  memcpy(destination, &source[startIndex], firstChunk * sizeof(uint16_t));
  memcpy(&destination[firstChunk], source, secondChunk * sizeof(uint16_t));
}

void ScreenDataHolder::updateSignalData(const uint8_t &dataType, const float &value)
{
  switch (dataType)
  {
    case VPP_VALUE:
      vpp = constrain(value, 0.0f, value);
      break;
      
    case V_RMS_VALUE:
      voltageRms = constrain(value, 0.0f, value);;
      break;
      
    case V_AVG_VALUE:
      voltageAvg = constrain(value, 0.0f, value);;
      break;
      
    case C_RMS_VALUE:
      currentRms = constrain(value, 0.0f, value);;
      break;
      
    case C_AVG_VALUE:
      currentAvg = constrain(value, 0.0f, value);;
      break;
      
    default:
      return;
  }
  newData = true;
}

int ScreenDataHolder::getMinVoltagePosition(size_t index)
{
  if (index < bufferSize) {
        return minVoltagePositions_[index];
    } else {
        Serial.println("Error: Index out of bounds");
        return 0.0f;
    }
}

int ScreenDataHolder::getMaxVoltagePosition(size_t index)
{
  if (index < bufferSize) {
        return maxVoltagePositions_[index];
    } else {
        Serial.println("Error: Index out of bounds");
        return 0.0f;
    }
}

void ScreenDataHolder::incrementMode() 
{
  mode = static_cast<Modes>((static_cast<uint8_t>(mode) + 1) % 3);
  uint8_t mode_casted = static_cast<uint8_t>(mode);
  saveParameter(modeParameterName, mode_casted);
  connection_->sendData((ScreenConnectionController::DataPair[]){
    ScreenConnectionController::DataPair(MODE_VALUE, mode_casted)
  }, 1);
  if (mode == Modes::AC)
    desiredVoltage = fabs(desiredVoltage);
}

void ScreenDataHolder::setValue()
{
  switch (editedValue) {
    case DataValues::Voltage:
      desiredVoltage = constrain(atof(valueToSet), voltageRange_[0], voltageRange_[1]);
      sendFloatValue(voltageParameterName, VOLTAGE_VALUE, desiredVoltage);
      break;
    case DataValues::VoltageSign:
      desiredVoltage *= -1.0f;
      sendFloatValue(voltageParameterName, VOLTAGE_VALUE, desiredVoltage);
      break;
    case DataValues::Frequency:
      desiredFrequency = constrain(atoi(valueToSet), frequencyRange_[0], frequencyRange_[1]);
      saveParameter(frequencyParameterName, desiredFrequency);
      connection_->sendData((ScreenConnectionController::DataPair[]){
        ScreenConnectionController::DataPair(FREQUENCY_VALUE, desiredFrequency)
      }, 1);

      break;
    case DataValues::Current:
      desiredCurrent = constrain(atof(valueToSet), currentRange_[0], currentRange_[1]);
      sendFloatValue(currentParameterName, CURRENT_VALUE, desiredVoltage);
      break;
    case DataValues::Length:
      wireLength = constrain(round(atof(valueToSet) * 2.0) / 2.0, lengthRange_[0], lengthRange_[1]);
      sendFloatValue(wireLengthParameterName, WIRE_VALUE, wireLength);
      break;
    default:
      return;
  }
}

void ScreenDataHolder::sendFloatValue(const char* paramName, const uint8_t &value_type, const float &value)
{
  saveParameter(paramName, value);
  connection_->sendData((ScreenConnectionController::DataPair[]){
    ScreenConnectionController::DataPair(value_type, value)
  }, 1);
}

void ScreenDataHolder::updateData()
{
  connection_->getEncoderData(useButton, switchButton);
  connection_->getLoadConnectedStatus(disconnected);
}

void ScreenDataHolder::setScreenColors(uint16_t _screenColor, uint16_t _textColor, uint16_t _backgroundTextColor)
{
  screenColor_ = _screenColor;
  textColor_ = _textColor;
  backgroundTextColor_ = _backgroundTextColor;
}

uint16_t ScreenDataHolder::getScreenColor()
{
  return screenColor_;
}

uint16_t ScreenDataHolder::getTextColor()
{
  return textColor_;
}

uint16_t ScreenDataHolder::getBackgroundTextColor()
{
  return backgroundTextColor_;
}

uint8_t ScreenDataHolder::getTextSize()
{
  return textSize_;
}

void ScreenDataHolder::setScreenColor(uint16_t color)
{
  screenColor_ = color;
}

void ScreenDataHolder::setTextColor(uint16_t color)
{
  textColor_ = color;
}

void ScreenDataHolder::setBackgroundTextColor(uint16_t color)
{
  backgroundTextColor_ = color;
}

void ScreenDataHolder::setTextSize(uint8_t size)
{
  textSize_ = size;
}

void ScreenDataHolder::preferencesBegin(bool readonly)
{
  if (!preferences_.begin(preferencesNamespace_, readonly))
  {
    Serial.println("Failed to open preferences_!");
    return;
  }
}

void ScreenDataHolder::preferencesEnd()
{
  preferences_.end();
}

void ScreenDataHolder::saveParameter(const char *param_name, uint8_t value)
{
  preferencesBegin(false);
  preferences_.putUChar(param_name, value);
  preferencesEnd();
}

void ScreenDataHolder::saveParameter(const char *param_name, uint16_t value)
{
  preferencesBegin(false);
  preferences_.putUShort(param_name, value);
  preferencesEnd();
}

void ScreenDataHolder::saveParameter(const char *param_name, float value)
{
  preferencesBegin(false);
  preferences_.putFloat(param_name, value);
  preferencesEnd();
}

void ScreenDataHolder::loadParameters()
{
  getSavedParameters();
  sendAllSettings(OPERATION_STOP);
}

void ScreenDataHolder::getSavedParameters()
{
  preferencesBegin(false);
  mode = static_cast<Modes>(preferences_.getUChar(modeParameterName, 0));
  desiredVoltage = preferences_.getFloat(voltageParameterName, 1.0f);
  desiredCurrent = preferences_.getFloat(currentParameterName, 10.0f);
  desiredFrequency = preferences_.getUShort(frequencyParameterName, 100);
  wireLength = preferences_.getFloat(wireLengthParameterName, 7.0f);
  preferencesEnd();
}

void ScreenDataHolder::sendAllSettings(const uint8_t &operation)
{
  connection_->sendData((ScreenConnectionController::DataPair[]){
    ScreenConnectionController::DataPair(MODE_VALUE, static_cast<uint8_t>(mode)),
    ScreenConnectionController::DataPair(FREQUENCY_VALUE, desiredFrequency),
    ScreenConnectionController::DataPair(VOLTAGE_VALUE, desiredVoltage),
    ScreenConnectionController::DataPair(CURRENT_VALUE, desiredCurrent),
    ScreenConnectionController::DataPair(WIRE_VALUE, wireLength),
    ScreenConnectionController::DataPair(OPERATION_VALUE, operation)
  }, 6);
}
