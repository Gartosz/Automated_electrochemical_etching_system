#include "ControllersHandler.h"

void ControllersHandler::generalInitializer()
{
  Wire.begin();
  delay(10);
  Wire.setClock(600000);
  delay(10);
  dac.begin(DAC_CS_PIN);
  dac.fastWriteA(0);

  setUpCurrentSensor();  
  delay(100);
}

void ControllersHandler::setUpCurrentSensor()
{
  uint8_t retryCount = 0;
  
  while (!ina219.begin() && retryCount < MAX_RETRIES)
  {
    Serial.print("Failed to find INA219 chip, attempt ");
    Serial.print(retryCount + 1);
    Serial.print(" of ");
    Serial.print(MAX_RETRIES);
    Serial.println(".");
    
    retryCount++;
    if (retryCount >= MAX_RETRIES)
    {
      Serial.println("Failed to connect to INA219 after maximum retries!");
      return;
    }
    delay(1000);
  }
  
  Serial.println("INA219 connected successfully!");
  if(calibrateCurrentSensor())
    Serial.println("INA219 calibrated successfully!");
  else
    Serial.println("Failed to calibrate INA219 - measurements might not be accurate!");
}

bool ControllersHandler::calibrateCurrentSensor()
{
  bool calibrationSuccessful = true;
  calibrationSuccessful &= ina219.setBusVoltageRange(16);
  calibrationSuccessful &= ina219.setGain(1);
  calibrationSuccessful &= ina219.setShuntResolution(10);
  calibrationSuccessful &= ina219.setMaxCurrentShunt(2, 0.1);
  calibrationSuccessful &= ina219.setModeShuntContinuous();
  return calibrationSuccessful;
}
