#include "ArduinoConnectionController.h"

void ArduinoConnectionController::begin(uint64_t baud)
{
  Serial1.begin(baud);
  pinMode(SCREEN_CS_PIN, OUTPUT);
  SPI.begin();
  digitalWrite(SCREEN_CS_PIN, HIGH);
  delay(10);
}

bool ArduinoConnectionController::getData(uint8_t* array, uint8_t &size)
{
  if (Serial1.available() < 6)
    return false;

  if (Serial1.read() != (BEGIN_UART_SEQUENCE >> 8))
    return false;

  if (Serial1.read() != (BEGIN_UART_SEQUENCE & 0xFF))
    return false;

  size = 0;
  uint8_t prev = 0;
  uint8_t current = 0;
  
  while (Serial1.available() > 0 && size < MAX_BUFFER_SIZE)
  {
    prev = current;
    current = Serial1.read();
    
    uint16_t sequence = (prev << 8) | current;
    if (sequence == FINISH_UART_SEQUENCE)
    {
      size--;
      return true;
    }
    
    array[size++] = current;
  }
  return false;
}

void ArduinoConnectionController::sendVoltages(uint16_t *voltagesArray, uint16_t voltagesArraySize, uint8_t valueType)
{
  SPI.beginTransaction(SCREEN_CS_PIN, screenSpiSettings);
  digitalWrite(SCREEN_CS_PIN, LOW);
  delayMicroseconds(10);
  SPI.transfer(TYPE_UINT16);
  SPI.transfer(valueType);
  SPI.transfer16(voltagesArraySize);
  for (uint16_t i = 0; i < voltagesArraySize; ++i)
    SPI.transfer16(voltagesArray[i]);

  delayMicroseconds(10);
  digitalWrite(SCREEN_CS_PIN, HIGH);
  SPI.endTransaction();
}

void ArduinoConnectionController::sendData(uint8_t *dataTypesArray, float *dataArray, uint8_t dataArraySize, bool disconnected)
{
  SPI.beginTransaction(SCREEN_CS_PIN, screenSpiSettings);
  digitalWrite(SCREEN_CS_PIN, LOW);
  delayMicroseconds(10);
  SPI.transfer(TYPE_FLOAT);
  SPI.transfer(dataArraySize);
  for (uint8_t i = 0; i < dataArraySize; ++i)
  {
    SPI.transfer(dataTypesArray[i]);
    delayMicroseconds(10);
    sendFloat(dataArray[i]);
  }
  sendUInt8(LOAD_DISCONNECTED);
  sendUInt8(disconnected);
  delayMicroseconds(10);
  digitalWrite(SCREEN_CS_PIN, HIGH);
  SPI.endTransaction();
}

void ArduinoConnectionController::sendMeasuredData(uint16_t *minVoltagesArray, uint16_t *maxVoltagesArray, uint8_t *dataTypesArray, float *dataArray, uint16_t voltagesArraySize, uint8_t dataArraySize, bool disconnected)
{
  sendData(dataTypesArray, dataArray, dataArraySize, disconnected);
  delay(10);
  sendVoltages(minVoltagesArray, voltagesArraySize, MIN_VOLTAGES_VALUE);
  delay(10);
  sendVoltages(maxVoltagesArray, voltagesArraySize, MAX_VOLTAGES_VALUE);
  delay(10);
}

void ArduinoConnectionController::sendUInt8(uint8_t value)
{
  SPI.transfer(TYPE_UINT8);
  SPI.transfer(value);
}

void ArduinoConnectionController::sendFloat(float value)
{
  uint8_t* bytes = (uint8_t*)&value;

  for (int i = 0; i < 4; ++i)
  {
    SPI.transfer(bytes[i]);
  }
}

void ArduinoConnectionController::sendEncoderData(const uint8_t &valueType, const uint8_t &pos)
{
  delay(1);
  SPI.beginTransaction(SCREEN_CS_PIN, screenSpiSettings);
  digitalWrite(SCREEN_CS_PIN, LOW);
  for (int i=0; i<2; ++i)
  {
    sendUInt8(valueType);
    sendUInt8(pos);
  }
  digitalWrite(SCREEN_CS_PIN, HIGH);
  SPI.endTransaction();
}