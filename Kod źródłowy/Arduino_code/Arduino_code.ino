#include "DataHolder.h"
#include "ControllersHandler.h"
#include "SignalManager.h"

ControllersHandler handler;
SignalManager manager(500, 8);
DataHolder data(&handler, &manager);
ArduinoConnectionController connection;

bool blockSpiTransfer = false;

void TC0_Handler() {
  TC_GetStatus(TC0, 0);
  ADC->ADC_CR = ADC_CR_START;
}

void ADC_Handler() {
  if (ADC->ADC_ISR & ADC_ISR_DRDY) {
      uint16_t sample = ADC->ADC_LCDR & 0xFFF;
      data.updateVoltageArray(sample);
    }
}


void setup()
{
  Serial.begin(250000);
  connection.begin(9600);
  handler.generalInitializer();
  manager.initialize();
}

void loop()
{
  unsigned long currentMs = millis();
  static uint16_t voltage = 0;
  static float currentMa = 0.0f;
  
  if(manager.handleEncoder(&data, handler.encoder.read(), currentMs) && !blockSpiTransfer)
  {
    if (data.useButton)
    {
      connection.sendEncoderData(ENCODER_BUTTON);
      data.useButton = false;
    }
    if (data.switchButton)
    {
      char buffer[10];
      connection.sendEncoderData(ENCODER_POSITION, data.switchButton);
      data.switchButton = 0;
    }
  }
  currentMa = handler.ina219.getCurrent_mA();
  data.updateCurrentArray(currentMa);
  
  if (!blockSpiTransfer)
    data.sendMeasuredData(connection);

  data.updateData(connection);

  manager.indicateLedStatus(data, currentMs);

  if (data.enableMotorForward)
    manager.operateMotor(currentMs, data.enableMotorForward, data.wireLength, MOTOR_ENABLE_FORWARD_PIN, blockSpiTransfer);
  else if (data.enableMotorBackward)
    manager.operateMotor(currentMs, data.enableMotorBackward, data.wireLength, MOTOR_ENABLE_BACKWARD_PIN, blockSpiTransfer);
}
