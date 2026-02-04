#pragma once

#include <Arduino.h>

namespace arduino::esp32::spi_::dma
{
  class Slave;
}

#define UART_TX 12
#define UART_RX -1
#define CLK_PIN 17
#define CS_PIN 18
#define MISO_PIN -1
#define MOSI_PIN 13
#define TYPE_UINT16 (uint8_t)0x01
#define TYPE_FLOAT (uint8_t)0x02
#define TYPE_UINT8 (uint8_t)0x03
#define OPERATING_DATA_VALUE (uint8_t)0xA0
#define ENCODER_POSITION (uint8_t)0xB0
#define ENCODER_BUTTON (uint8_t)0xB1
#define BUTTON_NEXT (uint8_t)0xAE
#define BUTTON_PREV (uint8_t)0xDE
#define MIN_VOLTAGES_VALUE (uint8_t)0xC0
#define MAX_VOLTAGES_VALUE (uint8_t)0xC1
#define LOAD_DISCONNECTED (uint8_t)0xCC
#define BEGIN_UART_SEQUENCE 0xABCD
#define FINISH_UART_SEQUENCE 0xFEDC
#define MAX_VOLTAGE_VALUES_SIZE 604
#define CHART_HEIGHT 308

class ScreenConnectionController
{
  public:

  struct DataPair {
    uint8_t valueType;
    union {
      uint8_t u8;
      uint16_t u16;
      float f;
    } value;
    enum { UINT8, UINT16, FLOAT } type;

    DataPair(uint8_t vt, uint8_t val) : valueType(vt), type(UINT8) { value.u8 = val; }
    DataPair(uint8_t vt, uint16_t val) : valueType(vt), type(UINT16) { value.u16 = val; }
    DataPair(uint8_t vt, float val) : valueType(vt), type(FLOAT) { value.f = val; }
  };

  ScreenConnectionController();
  ~ScreenConnectionController();
  constexpr static uint16_t BUFFER_ARRAY_SIZE = 2048;
  constexpr static uint16_t RX_BUFFER_SIZE = 2048;
  constexpr static uint8_t TX_BUFFER_SIZE = 32;
  volatile static size_t count;
  size_t countMissed = 0;
  bool getSignalData(uint8_t &dataType, float &dataValue);
  void getVoltageData(uint16_t **minVoltageArray, uint16_t **maxVoltageArray,
                      uint16_t &readMinVoltageStartIndex, uint16_t &readMaxVoltageStartIndex, uint16_t &dataSize);
  void getEncoderData(bool &buttonPressed, int8_t &encoderMove);
  void getLoadConnectedStatus(bool &disconnected);
  void begin(TaskHandle_t *voltageUpdateTaskToCall, TaskHandle_t *currentUpdateTaskToCall,
             TaskHandle_t *connectionHandleTaskToCall, uint64_t baud);
  void sendData(const DataPair* pairs, const uint8_t &count);
  void trigger();

  private:
  struct TypeValuePair {
    uint8_t type;
    float value;
  };

  arduino::esp32::spi_::dma::Slave *spi_;
  TaskHandle_t *voltageUpdateTask_ = nullptr;
  TaskHandle_t *signalDataUpdateTask_ = nullptr;
  TaskHandle_t *connectionHandleTask_ = nullptr;
  QueueHandle_t dataQueue_;
  static float dataValue_;
  static uint16_t minVoltageArray_[BUFFER_ARRAY_SIZE];
  static uint16_t maxVoltageArray_[BUFFER_ARRAY_SIZE];
  constexpr static uint16_t voltageDataSize_ = 604;
  static uint16_t nextMinVoltageReceiveIndex_;
  static uint16_t nextMaxVoltageReceiveIndex_;
  constexpr static uint8_t maxTransactionsCount_ = 10;
  static uint8_t dataType_;
  static uint8_t transactionCounter_;
  uint16_t readMinVoltageDataStartIndex_ = 0;
  uint16_t readMaxVoltageDataStartIndex_ = 0;
  int8_t encoderMove_ = 0;
  uint8_t *rxBuffer_;
  uint8_t *txBuffer_;
  bool buttonPressed_ = false;
  bool loadDisconnected_ = false;
  void transmittData(const DataPair* pairs, const uint8_t &count);
  void sendData(const uint8_t &valueType, const uint8_t &value);
  void sendData(const uint8_t &valueType, const uint16_t &value);
  void sendData(const uint8_t &valueType, const float &value);
  void decodeRxBuffer(const size_t &bytesCount);
};
