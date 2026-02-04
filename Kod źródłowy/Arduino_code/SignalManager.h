#pragma once

#include "DataHolder.h"
#include "ControllersHandler.h"

#define VOLTAGE_MEASUREMENT_PIN A0
#define MOTOR_PWM_PIN 12
#define MOTOR_ENABLE_FORWARD_PIN 26
#define MOTOR_ENABLE_BACKWARD_PIN 24
#define FREQUENCY_SET_PIN 6
#define GREEN_LED_PIN 23
#define RED_LED_PIN 25
#define ENC_SW_PIN 22
#define MOTOR_PWM_VALUE 90
#define MOTOR_TOGGLE_PREVENTION_PERIOD 1000

class SignalManager
{
public:
  SignalManager(uint16_t _encoderLockTime, uint8_t _prescaler = 8):
  encoderLockTime_(_encoderLockTime), prescaler_(_prescaler) {}

  bool handleEncoder(DataHolder *data, int32_t newPosition, const uint64_t &currentMs);
  void initialize();
  void setUpOperation(DataHolder &data);
  void stopOperation(DataHolder &data);
  void operateMotor(uint64_t currentMs, bool &enableMotor, const float &wireLength, const uint8_t &motorEnablePin, bool &blockSpiTransfer);
  void indicateLedStatus(DataHolder &data, const uint64_t &currentMs);
  void setVoltage(const float &voltage, ControllersHandler *handler);
  void setFrequency(uint16_t frequency = 100, const uint8_t &dutyCycle = 50);

private:
  const uint64_t offLedTimeout_ = 2000;
  const uint16_t encoderLockTime_;
  const uint16_t blinkInterval_ = 500;
  const uint8_t prescaler_;
  uint64_t motorStartTime_ = 0;
  uint64_t stopLedTimer_ = 0;
  uint64_t startTime_ = 0;
  uint64_t motorLastActivationTime_ = 0;
  uint64_t lastEncoderUseTime_ = 0;
  uint32_t pwmPeriod_ = 0;
  uint32_t topValue_;
  int32_t encoderPosition_ = 0;
  void setPins();
  void setRegisters();
  void setDutyCycle(uint8_t percentage);
  void blink(const uint64_t &currentMs);
  void updatePosition(DataHolder *data, int32_t newPosition);
};
