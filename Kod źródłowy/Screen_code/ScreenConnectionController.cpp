#include "ScreenConnectionController.h"
#include <ESP32DMASPISlave.h>

uint16_t ScreenConnectionController::nextMinVoltageReceiveIndex_ = 0;
uint16_t ScreenConnectionController::nextMaxVoltageReceiveIndex_ = 0;
uint8_t ScreenConnectionController::dataType_ = 0;
float ScreenConnectionController::dataValue_ = 0.0f;
uint16_t ScreenConnectionController::minVoltageArray_[BUFFER_ARRAY_SIZE] = {0};
uint16_t ScreenConnectionController::maxVoltageArray_[BUFFER_ARRAY_SIZE] = {0};
uint8_t ScreenConnectionController::transactionCounter_ = 0;
volatile size_t ScreenConnectionController::count = 0;

ScreenConnectionController::ScreenConnectionController(): spi_(new ESP32DMASPI::Slave()), dataQueue_(xQueueCreate(10, sizeof(TypeValuePair)))
{
  txBuffer_ = spi_->allocDMABuffer(RX_BUFFER_SIZE);
  rxBuffer_ = spi_->allocDMABuffer(RX_BUFFER_SIZE);
}

ScreenConnectionController::~ScreenConnectionController()
{
  delete spi_;
}

void ScreenConnectionController::begin(TaskHandle_t *voltageUpdateTaskToCall, TaskHandle_t *currentUpdateTaskToCall, TaskHandle_t *connectionHandleTaskToCall, uint64_t baud)
{
  Serial1.begin(baud, SERIAL_8N1, UART_RX, UART_TX);
  spi_->setDataMode(SPI_MODE0);
  spi_->setQueueSize(1);
  spi_->begin(HSPI, CLK_PIN, MISO_PIN, MOSI_PIN, CS_PIN);
  voltageUpdateTask_ = voltageUpdateTaskToCall;
  signalDataUpdateTask_ = currentUpdateTaskToCall;
  connectionHandleTask_ = connectionHandleTaskToCall;
}

bool ScreenConnectionController::getSignalData(uint8_t &dataType, float &dataValue)
{
  TypeValuePair data;
  
  if (xQueueReceive(dataQueue_, &data, 0) == pdTRUE)
  {
    dataType = data.type;
    dataValue = data.value;
    return true;
  }
  return false;
}

void ScreenConnectionController::getVoltageData(uint16_t **minVoltageArray, uint16_t **maxVoltageArray, uint16_t &readMinVoltageStartIndex, uint16_t &readMaxVoltageStartIndex, uint16_t &arraySize)
{
  *minVoltageArray = minVoltageArray_;
  *maxVoltageArray = maxVoltageArray_;
  readMinVoltageStartIndex = readMinVoltageDataStartIndex_;
  readMaxVoltageStartIndex = readMaxVoltageDataStartIndex_;
  arraySize = BUFFER_ARRAY_SIZE;
  readMinVoltageDataStartIndex_ = (readMinVoltageDataStartIndex_ + voltageDataSize_) % BUFFER_ARRAY_SIZE;
  readMaxVoltageDataStartIndex_ = (readMaxVoltageDataStartIndex_ + voltageDataSize_) % BUFFER_ARRAY_SIZE;
  if (transactionCounter_ == maxTransactionsCount_)
  {
    readMinVoltageStartIndex = 0;
    readMaxVoltageStartIndex = 0;
    readMinVoltageDataStartIndex_ = 0;
    readMaxVoltageDataStartIndex_ = 0;
  }
}

void ScreenConnectionController::getEncoderData(bool &buttonPressed, int8_t &encoderMove)
{
  if (encoderMove_)
    encoderMove = encoderMove_;
    encoderMove_ = 0;
  if (buttonPressed_)
    buttonPressed = buttonPressed_;
    buttonPressed_ = false;
}

void ScreenConnectionController::getLoadConnectedStatus(bool &disconnected)
{
  disconnected = loadDisconnected_;
}

void ScreenConnectionController::sendData(const DataPair* pairs, const uint8_t &count)
{
  for (size_t i = 0; i < 5; ++i)
  {
    transmittData(pairs, count);
    delay(5);
  }
}

void ScreenConnectionController::transmittData(const DataPair* pairs, const uint8_t &count)
{
  Serial1.write(BEGIN_UART_SEQUENCE >> 8);
  Serial1.write(BEGIN_UART_SEQUENCE & 0xFF);

  for (uint8_t i = 0; i < count; i++)
  {
    switch(pairs[i].type)
    {
      case DataPair::UINT8:
        sendData(pairs[i].valueType, pairs[i].value.u8);
        break;
      case DataPair::UINT16:
        sendData(pairs[i].valueType, pairs[i].value.u16);
        break;
      case DataPair::FLOAT:
        sendData(pairs[i].valueType, pairs[i].value.f);
        break;
    }
  }

  Serial1.write(FINISH_UART_SEQUENCE >> 8);
  Serial1.write(FINISH_UART_SEQUENCE & 0xFF);
}

void ScreenConnectionController::sendData(const uint8_t &valueType, const uint8_t &value)
{
  Serial.println("sending uint8");
  Serial1.write(TYPE_UINT8);
  Serial1.write(valueType);
  Serial1.write(TYPE_UINT8);
  Serial1.write(value);
}

void ScreenConnectionController::sendData(const uint8_t &valueType, const uint16_t &value)
{
  Serial.println("sending uint16");
  Serial1.write(TYPE_UINT8);
  Serial1.write(valueType);
  Serial1.write(TYPE_UINT16);
  Serial1.write((uint8_t*)&value, sizeof(value));
}

void ScreenConnectionController::sendData(const uint8_t &valueType, const float &value)
{
  Serial.println("sending float");
  // 0.0001f added as there were issues with zeros in returned bytes on Arduino
  float temp = value + 0.0001f;
  Serial1.write(TYPE_UINT8);
  Serial1.write(valueType);
  Serial1.write(TYPE_FLOAT);
  Serial1.write((uint8_t*)&temp, sizeof(temp));
}

void ScreenConnectionController::trigger()
{
  size_t bytesCount = 0;
  while (true)
  {
    if (spi_->hasTransactionsCompletedAndAllResultsHandled())
    {
      memset(rxBuffer_, 0, RX_BUFFER_SIZE);
      spi_->queue(txBuffer_, rxBuffer_, RX_BUFFER_SIZE);
      spi_->trigger();
    }

    if (spi_->hasTransactionsCompletedAndAllResultsReady(1))
    {
      bytesCount = spi_->numBytesReceived();
      decodeRxBuffer(bytesCount);
<<<<<<< Updated upstream
    }
    vTaskDelay(2);
  }
=======
    } 
    vTaskDelay(1);
  }

>>>>>>> Stashed changes
}

void ScreenConnectionController::decodeRxBuffer(const size_t &bytesCount)
{
  uint16_t index = 0;
  static uint64_t uint_count = 0;
  
  while(rxBuffer_[index] != 0 && index < bytesCount)
  {
    switch (rxBuffer_[index++])
    {
        case TYPE_UINT16:
        {
          uint8_t arrayType = rxBuffer_[index++];
          uint16_t size = (rxBuffer_[index++] << 8) | rxBuffer_[index++];
          
          if(size == 0 || size > MAX_VOLTAGE_VALUES_SIZE)
          {
            return;
          }
          
          uint16_t *voltageArray = arrayType == MIN_VOLTAGES_VALUE ? minVoltageArray_ : 
                                   arrayType == MAX_VOLTAGES_VALUE ? maxVoltageArray_ : nullptr;
          uint16_t *nextVoltageIndex = arrayType == MIN_VOLTAGES_VALUE ? &nextMinVoltageReceiveIndex_ : 
                                       arrayType == MAX_VOLTAGES_VALUE ? &nextMaxVoltageReceiveIndex_ : nullptr;
          
          if(!voltageArray || !nextVoltageIndex)
          {
            return;
          }
          
          uint_count += size;
          
          for (size_t i=0; i<size; ++i)
          {
            uint16_t voltagePosition = (rxBuffer_[index + i * sizeof(uint16_t)] << 8) | 
                                       rxBuffer_[index + i * sizeof(uint16_t) + 1];
            voltageArray[(*nextVoltageIndex + i) % BUFFER_ARRAY_SIZE] = constrain(voltagePosition, 0, CHART_HEIGHT);
          }
          
          index += sizeof(uint16_t) * size;
          *nextVoltageIndex = (*nextVoltageIndex + size) % BUFFER_ARRAY_SIZE;
          
          break;
        }
        case TYPE_UINT8:
        {
          uint8_t value = rxBuffer_[index++];
          if (rxBuffer_[index++] == TYPE_UINT8)
          {
            if (value == ENCODER_POSITION && (rxBuffer_[index] == BUTTON_NEXT || rxBuffer_[index] == BUTTON_PREV))
            {
              encoderMove_ = (rxBuffer_[index] == BUTTON_NEXT) ? 1 : -1;
              Serial.printf("Button move %i %i\n", encoderMove_, rxBuffer_[index]);
              index = RX_BUFFER_SIZE;
            }
            else if (value == ENCODER_BUTTON && rxBuffer_[index] == 1)
            {
              buttonPressed_ = true;
              index = RX_BUFFER_SIZE;
              Serial.println("Button press");
            }
            else if (value == LOAD_DISCONNECTED)
            {
              loadDisconnected_ = rxBuffer_[index];
              Serial.printf("Load %i\n", loadDisconnected_);
            }
          }
          else
            index++;
          break;
        }
        case TYPE_FLOAT:
        {
          uint8_t size = rxBuffer_[index++];
          for (size_t i = 0; i < size; ++i)
          {
            TypeValuePair data;
            data.type = rxBuffer_[index++];
            memcpy(&data.value, &rxBuffer_[index], sizeof(float));
            index += sizeof(float);
            xQueueSend(dataQueue_, &data, 0);
          }
          xTaskNotifyGive(*signalDataUpdateTask_);
          break;
        }
        default:
          break;
    }
    
    if(index >= RX_BUFFER_SIZE) {
      break;
    }
  }
  
  uint16_t minAvailable = (nextMinVoltageReceiveIndex_ - readMinVoltageDataStartIndex_ + BUFFER_ARRAY_SIZE) % BUFFER_ARRAY_SIZE;
  uint16_t maxAvailable = (nextMaxVoltageReceiveIndex_ - readMaxVoltageDataStartIndex_ + BUFFER_ARRAY_SIZE) % BUFFER_ARRAY_SIZE;
  
  if (minAvailable >= voltageDataSize_ && maxAvailable >= voltageDataSize_)
  {
    uint_count = 0;
    xTaskNotify(*voltageUpdateTask_, NULL, eNoAction);
  }
}
