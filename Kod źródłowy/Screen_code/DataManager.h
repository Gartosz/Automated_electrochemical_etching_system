#pragma once

#include "ScreenDataHolder.h"

#define EPSILON 0.01
#define TOLERANCE_CURRENT 1
#define TOLERANCE_VOLTAGE 0.15

class DataManager
{
public:
  void handleOperation(ScreenDataHolder &data, const uint64_t &currentMs);
  void endSettings(ScreenDataHolder &data); 

private:
  const uint32_t maxOperatingTime_ = 900000;
  const uint16_t alignParmeterTime_ = 10000;
  const uint16_t dropReactionTime_ = 3000;
  const uint16_t disconnectReactionTime_ = 2500;
  const uint8_t currentUpdateTime_ = 50;
  uint64_t stoppingConditionTimer_ = 0;
  uint64_t startTime_ = 0;
  uint64_t lastCurrentAdjustment_ = 0;
  bool stopped_ = true;

  float calculateResistance(ScreenDataHolder &data);
  void validateACModeOperatingConditions(ScreenDataHolder &data, const uint64_t &currentMs);
  void validateVoltageRelatedConditions(ScreenDataHolder &data, const uint64_t &currentMs);
  void validateVCCSConditions(ScreenDataHolder &data, const uint64_t &currentMs);
  void keepCurrent(ScreenDataHolder &data, const uint64_t &currentMs);
};
