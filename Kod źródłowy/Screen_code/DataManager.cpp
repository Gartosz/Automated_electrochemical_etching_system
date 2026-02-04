#include "DataManager.h"

void DataManager::handleOperation(ScreenDataHolder &data, const uint64_t &currentMs)
{
  if (startTime_ == 0)
  {
    startTime_ = currentMs;
    stopped_ = false;
  }
  if (currentMs - startTime_ > maxOperatingTime_)
  {
    data.code = ScreenDataHolder::ExitCodes::Timeout;
    endSettings(data);
    return;
  }
  else if (data.disconnected && currentMs - startTime_ > disconnectReactionTime_)
  {
    data.code = ScreenDataHolder::ExitCodes::LoadDisconnected;
    endSettings(data);
    return;
  }
  switch(data.mode)
  {
    case ScreenDataHolder::Modes::AC:
      if (data.setParameter)
      {
        data.setParameter = false;
        stoppingConditionTimer_ = 0;
        data.desiredVoltage = fabs(data.desiredVoltage);
        data.sendExecutiveData((ScreenConnectionController::DataPair[]){
          ScreenConnectionController::DataPair(VOLTAGE_VALUE, data.desiredVoltage),
          ScreenConnectionController::DataPair(FREQUENCY_VALUE, data.desiredFrequency)
        }, 2);
      }
      validateACModeOperatingConditions(data, currentMs);
    break;
    case ScreenDataHolder::Modes::DC:
      if (data.setParameter)
      {
        data.sendExecutiveData((ScreenConnectionController::DataPair[]){
          ScreenConnectionController::DataPair(VOLTAGE_VALUE, data.desiredVoltage)
        }, 1);
        data.setParameter = false;
        stoppingConditionTimer_ = 0;
      }
      validateVoltageRelatedConditions(data, currentMs);
    break;
    case ScreenDataHolder::Modes::VCCS:
      if (data.setParameter)
      {
        data.sendExecutiveData((ScreenConnectionController::DataPair[]){
          ScreenConnectionController::DataPair(VOLTAGE_VALUE, data.desiredVoltage)
        }, 1);
        data.setParameter = false;
        stoppingConditionTimer_ = 0;
      }
      keepCurrent(data, currentMs);
      validateVCCSConditions(data, currentMs);
    break;
    default:
    break;
  }
}

void DataManager::validateVoltageRelatedConditions(ScreenDataHolder &data, const uint64_t &currentMs)
{
  float measuredVoltage = data.voltageRms - fabs(data.voltageAvg) <= EPSILON ? data.voltageAvg : data.voltageRms;
  bool voltageOutOfTolerance = fabs(measuredVoltage - data.desiredVoltage) > TOLERANCE_VOLTAGE;

  if (voltageOutOfTolerance && stoppingConditionTimer_ == 0)
    stoppingConditionTimer_ = currentMs;
  else if (data.validateVoltage && voltageOutOfTolerance && currentMs - stoppingConditionTimer_ >= alignParmeterTime_)
  {
    data.code = ScreenDataHolder::ExitCodes::VoltageOutOfRange;
    endSettings(data);
  }
  else if (!voltageOutOfTolerance)
    stoppingConditionTimer_ = 0;
}

void DataManager::validateACModeOperatingConditions(ScreenDataHolder &data, const uint64_t &currentMs)
{
  bool currentOutOfTolerance = data.currentRms < data.desiredCurrent - TOLERANCE_CURRENT;

  if (data.validateCurrent && currentOutOfTolerance && currentMs - startTime_ > dropReactionTime_)
  {
    data.code = ScreenDataHolder::ExitCodes::CurrentDropped;
    endSettings(data);
  }
  else
    validateVoltageRelatedConditions(data, currentMs);
}

void DataManager::validateVCCSConditions(ScreenDataHolder &data, const uint64_t &currentMs)
{
  float measuredVoltage = data.voltageRms - fabs(data.voltageAvg) <= EPSILON ? data.voltageAvg : data.voltageRms;
  bool voltageOutOfTolerance = (measuredVoltage * data.desiredVoltage) < 0 || fabs(measuredVoltage) < fabs(data.desiredVoltage) - TOLERANCE_VOLTAGE;
  float measuredCurrent = data.currentRms - fabs(data.currentAvg) <= EPSILON ? data.currentAvg : data.currentRms;
  bool currentOutOfTolerance = fabs(measuredCurrent - data.desiredCurrent) > TOLERANCE_CURRENT;

  if (currentOutOfTolerance && stoppingConditionTimer_ == 0)
    stoppingConditionTimer_ = currentMs;
  else if (data.validateCurrent && currentOutOfTolerance && currentMs - stoppingConditionTimer_ >= alignParmeterTime_)
  {
    data.code = ScreenDataHolder::ExitCodes::CurrentOutOfRange;
    endSettings(data);
  }
  else if (!currentOutOfTolerance)
    stoppingConditionTimer_ = 0;

  if (data.validateVoltage &&voltageOutOfTolerance && currentMs - startTime_ > dropReactionTime_)
  {
    data.code = ScreenDataHolder::ExitCodes::VoltageDropped;
    endSettings(data);
  }
}

void DataManager::keepCurrent(ScreenDataHolder &data, const uint64_t &currentMs) 
{
  static float voltage = 0.1f;
  if (currentMs - lastCurrentAdjustment_ <= currentUpdateTime_)
    return;
  lastCurrentAdjustment_ = currentMs;
  voltage = calculateResistance(data) * data.desiredCurrent / 1000;
  voltage = constrain(voltage, 0.01f, 5.0f);
  data.sendExecutiveData((ScreenConnectionController::DataPair[]){
    ScreenConnectionController::DataPair(VOLTAGE_VALUE, data.desiredVoltage)
  }, 1);
}

float DataManager::calculateResistance(ScreenDataHolder &data)
{
  return data.voltageRms / data.currentRms * 1000;
}

void DataManager::endSettings(ScreenDataHolder &data)
{
  if (!stopped_)
  {
    data.sendExecutiveData((ScreenConnectionController::DataPair[]){
      ScreenConnectionController::DataPair(OPERATION_VALUE, OPERATION_STOP)
    }, 1);
    data.operating = false;
    startTime_ = 0;
    stoppingConditionTimer_ = 0;
    stopped_ = true;
  }
}
