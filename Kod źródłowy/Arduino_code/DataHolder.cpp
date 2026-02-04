#include "DataHolder.h"
#include "ControllersHandler.h"
#include "SignalManager.h"
#include "math.h"

void DataHolder::updateData(ArduinoConnectionController &connection)
{
  uint8_t rxBuffer[MAX_BUFFER_SIZE];
  uint8_t size;
  if(!connection.getData(rxBuffer, size))
    return;
  parseData(rxBuffer, size);
}

void DataHolder::parseData(const uint8_t * const buffer, const uint8_t &size)
{
  for (size_t i = 0; i < size; ++i)
  {
    if (buffer[i] == TYPE_UINT8 && i+1 < size)
      i += setData(buffer[i+1], buffer[i+2], &buffer[i+3]) + 2;
  }
  if (operating)
    manager->setUpOperation(*this);
}

uint8_t DataHolder::setData(uint8_t dataType, uint8_t valueType, const uint8_t *value)
{
  switch(dataType)
  {
    case OPERATION_VALUE:
      if (valueType == TYPE_UINT8)
        setOperation(value[0]);
      return sizeof(uint8_t);
    case MODE_VALUE:
      if (valueType == TYPE_UINT8)
        setMode(value[0]);
      return sizeof(uint8_t);
    case VOTLAGE_VALUE:
      if (valueType == TYPE_FLOAT)
      {
        desiredVoltage = constrain(parseFloat(value), voltageRange_[0], voltageRange_[1]);
        if (operating)
          manager->setVoltage(desiredVoltage, handler);
      }
      return sizeof(float);
    case CURRENT_VALUE:
      if (valueType == TYPE_FLOAT)
      {
        desiredCurrent = constrain(parseFloat(value), currentRange_[0], currentRange_[1]);
      }
      return sizeof(float);
    case FREQUENCY_VALUE:
      if (valueType == TYPE_UINT16)
      {
        uint16_t frequency;
        memcpy(&frequency, value, sizeof(uint16_t));
        desiredFrequency = constrain(frequency, frequencyRange_[0], frequencyRange_[1]);
        if (operating)
          manager->setFrequency(desiredFrequency);
      }
      return sizeof(uint16_t);
    case WIRE_VALUE:
      if (valueType == TYPE_FLOAT)
      {
        float parsedLength = parseFloat(value);
        wireLength = constrain(round(parsedLength * 2.0) / 2.0, lengthRange_[0], lengthRange_[1]);
      }
      return sizeof(float);
    case ACTIVATE_MOTOR_EXTRUDE:
        enableMotorForward = true;
        return sizeof(uint8_t);
    case ACTIVATE_MOTOR_RETRACT:
        enableMotorBackward = true;
        return sizeof(uint8_t);
    default:
      return 0;
  }
}

float DataHolder::parseFloat(const uint8_t* value)
{
  float result;
  memcpy(&result, value, sizeof(float));
  result -= 0.0001f;
  return result;
}

void DataHolder::setOperation(uint8_t value)
{
  switch (value)
  {
    case OPERATION_START:
      waiting = false;
      operating = true;
      manager->setUpOperation(*this);
      break;
    case OPERATION_STOP:
      waiting = false;
      operating = false;
      manager->stopOperation(*this);
      break;
    case OPERATION_STANDBY:
      waiting = false;
      break;
    case OPERATION_WAITING:
      waiting = true;
      break;
    default:
      break;
  }
}

void DataHolder::updateVoltageArray(uint16_t voltage)
{
  voltageArray_[indexVoltage_] = voltage;
  indexVoltage_ = (indexVoltage_ + 1) % voltageArraySize;
}

void DataHolder::updateCurrentArray(float current)
{
  currentArray_[indexCurrent_] = current;
  indexCurrent_ = (indexCurrent_ + 1) % currentArraySize;
}

void DataHolder::sendMeasuredData(ArduinoConnectionController &connection)
{
  if (indexVoltage_ >= voltageArraySize * 0.95)
    {
      updateVoltageData();
      updateCurrentData();
      indexCurrent_ = 0;
      indexVoltage_ = 0;
      uint8_t dataTypes[5] = {VPP_VALUE, V_RMS_VALUE, V_AVG_VALUE, C_RMS_VALUE, C_AVG_VALUE};
      float data[5] = {vpp_, voltageRms_, voltageAvg_, currentRms_, currentAvg_};
      connection.sendMeasuredData(minVoltagePositions_, maxVoltagePositions_, dataTypes, data, CHART_WIDTH, 5, disconnected_);
    }
}

void DataHolder::updateVoltageData()
{
  uint16_t valuesCount = 0;
  voltageSumOfSquares_ = 0.0f;
  voltageAvg_ = 0.0f;
  maxVoltage_ = voltageRange_[0];
  minVoltage_ = voltageRange_[1];
  processSamplesForChart(valuesCount);
  voltageAvg_ /= valuesCount;
  voltageRms_ = sqrt(max(0.0f, voltageSumOfSquares_) / valuesCount);
  vpp_ = fabs(maxVoltage_ - minVoltage_);
}

void DataHolder::processSamplesForChart(uint16_t &valuesCount)
{
  const float DESIRED_TIME_WINDOW = 0.1f;
  const float SAMPLING_RATE = 100000.0f;
  
  float samplesInWindow = SAMPLING_RATE * DESIRED_TIME_WINDOW;
  float scale = samplesInWindow / float(indexVoltage_ + 1);
  
  float samplesPerPixel = float(indexVoltage_ + 1) / float(CHART_WIDTH) * scale;
  
  for (size_t x = 0; x < CHART_WIDTH; ++x)
  {
    float sampleStart = x * samplesPerPixel;
    float sampleEnd = (x + 1) * samplesPerPixel;
    
    int startIdx = int(sampleStart);
    int endIdx = int(sampleEnd);
    int samplesToProcess = max(1, endIdx - startIdx);
    
    float minValue = 5.0f;
    float maxValue = -5.0f;
    
    for (int i = 0; i < samplesToProcess; i++)
    {
      int actualIndex = (startIdx + i);
      uint16_t sample = voltageArray_[actualIndex];
      float voltageValue = ((sample * 3.3f / 4095.0f) - 1.584f) / 0.300f;
      if (voltageValue < minValue) minValue = voltageValue;
      if (voltageValue > maxValue) maxValue = voltageValue;
      updateCalculationsForSample(voltageValue);
      ++valuesCount;
    }

    minVoltagePositions_[x] = constrain(samplesToVerticalCord(minValue), 0, CHART_HEIGHT);
    maxVoltagePositions_[x] = constrain(samplesToVerticalCord(maxValue), 0, CHART_HEIGHT);
  }
}

void DataHolder::updateCalculationsForSample(const float &voltageValue)
{
  voltageAvg_ += voltageValue;
  voltageSumOfSquares_ += voltageValue * voltageValue;
  maxVoltage_ = maxVoltage_ > voltageValue ? maxVoltage_ : voltageValue;
  minVoltage_ = minVoltage_ < voltageValue ? minVoltage_ : voltageValue;
}

uint16_t DataHolder::samplesToVerticalCord(const float &voltageValue)
{
  static float displayMin = -5.0f - VERTICAL_PADDING_VOLTAGE;
  static float displayMax = 5.0f + VERTICAL_PADDING_VOLTAGE;
  return CHART_HEIGHT - (uint16_t)(((voltageValue - displayMin) / (displayMax - displayMin)) * CHART_HEIGHT);
}

void DataHolder::updateCurrentData()
{
  static float previousCurrent = 0.0f;
  static uint8_t disconnectedCounter = 0;
  currentSumOfSquares_ = 0;
  currentAvg_ = 0;
  for (size_t i = 0; i < indexCurrent_ + 1; ++i)
  {
    float currentValue = currentArray_[(i)];
    currentAvg_ += currentValue;
    currentSumOfSquares_ += currentValue * currentValue;
    countDisconnectedTime(currentValue, previousCurrent, disconnectedCounter);
    previousCurrent = currentValue;
  }
  currentAvg_ /= indexCurrent_ + 1;
  currentRms_ = sqrt(max(0.0f, currentSumOfSquares_) / indexCurrent_ + 1);
}

void DataHolder::countDisconnectedTime(float previousCurrent, const float &readCurrent, uint8_t &disconnectedCounter)
{
  if (fabs(previousCurrent) + fabs(readCurrent) < 0.5)
    ++disconnectedCounter;
  else
  {
    disconnectedCounter = 0;
    disconnected_ = false;
    return;
  }
  checkIfDisconnected(disconnectedCounter);
}

void DataHolder::checkIfDisconnected(uint8_t &disconnectedCounter)
{
  if (disconnectedCounter >= disconnectedCountLimit_)
    disconnected_ = true;
  else
    disconnected_ = false;
}
