#pragma once

#include "INA219.h"
#include "MCP_DAC.h"
#include "Encoder.h"

#define DAC_CS_PIN 52
#define ENC_DT_PIN 30
#define ENC_CLK_PIN 32
#define MAX_RETRIES 5
#define INA219_I2C_ADDRESS 0x40

class ControllersHandler
{
public:
  INA219 ina219;
  MCP4921 dac;
  Encoder encoder = Encoder(ENC_DT_PIN, ENC_CLK_PIN);

  ControllersHandler(): ina219(INA219_I2C_ADDRESS) {}

  void generalInitializer();

private:
    void setUpCurrentSensor();
    bool calibrateCurrentSensor();
};
