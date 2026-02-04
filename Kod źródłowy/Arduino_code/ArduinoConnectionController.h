#pragma once

#include <Arduino.h>
#include <SPI.h>

#define SCREEN_CS_PIN 10
#define I2C_ADDR 0x30
#define MAX_RETRIES 5
#define TYPE_UINT16 (uint8_t)0x01
#define TYPE_FLOAT (uint8_t)0x02
#define TYPE_UINT8 (uint8_t)0x03
#define OPERATING_DATA_VALUE (uint8_t)0xAA
#define MIN_VOLTAGES_VALUE (uint8_t)0xC0
#define MAX_VOLTAGES_VALUE (uint8_t)0xC1
#define LOAD_DISCONNECTED (uint8_t)0xCC
#define BUTTON_NEXT (uint8_t)0xAE
#define BUTTON_PREV (uint8_t)0xDE
#define BEGIN_UART_SEQUENCE 0xABCD
#define FINISH_UART_SEQUENCE 0xFEDC
#define MAX_BUFFER_SIZE 256

class ArduinoConnectionController
{
  public:
  ArduinoConnectionController(): screenSpiSettings(1000000, MSBFIRST, SPI_MODE0) {}
  bool getData(uint8_t* array, uint8_t &size);
  void begin(uint64_t baud = 250000);
  void sendMeasuredData(uint16_t *minVoltagesArray, uint16_t *maxVoltagesArray,
                        uint8_t *dataTypesArray, float *dataArray, uint16_t voltageArraySize, uint8_t dataArraySize, bool disconnected);
  void sendEncoderData(const uint8_t &valueType, const uint8_t &pos = 1);

  private:
  SPISettings screenSpiSettings;
  void verifyScreenConnectionI2C();
  void sendMeasuredData(const uint16_t &voltage, const float &current);
  void sendVoltages(uint16_t *voltagesArray, uint16_t voltagesArraySize, uint8_t valueType);
  void sendData(uint8_t *dataTypesArray, float *dataArray, uint8_t dataArraySize, bool disconnected);
  void sendUInt8(uint8_t value);
  void sendFloat(float value);
};
