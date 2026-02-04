#include "TasksManager.h"

void TasksManager::startConnectionTask(TaskHandle_t *task)
{
  xTaskCreatePinnedToCore(
      triggerTask,
      "SPI Task", 
      8092,
      &context_,
      2,
      task,
      0
  );
}

void TasksManager::triggerTask(void *parameter)
{
  ScreenConnectionController *connection = ((TasksContext*)parameter)->connection;
  connection->trigger();
}

void TasksManager::startTouchLoopTask()
{
  xTaskCreatePinnedToCore(
      checkTouchTask,
      "Touchscreen Task",
      8092,
      &context_,
      0,
      NULL,
      0
  );
}

void TasksManager::checkTouchTask(void *parameter)
{
  ScreenManager *manager = ((TasksContext*)parameter)->screenManager;
  manager->checkTouch();
}

void TasksManager::processTouchTask(void *parameter)
{
  TasksContext *ctx = (TasksContext*)parameter;
  while(true)
  {
    xTaskNotifyWait(0, ULONG_MAX, NULL, portMAX_DELAY);
    unsigned long currentMs = millis();
    if (ctx->screenManager->canTouchBeProcessed(currentMs) && xSemaphoreTake(ctx->mutex, portMAX_DELAY))
    {
      int touch_x, touch_y;
      ctx->screenManager->getCoordinates(touch_x, touch_y, currentMs);
      ctx->UI->processClick(touch_x, touch_y, ctx->screenManager->gfx, ctx->data);
      xSemaphoreGive(ctx->mutex);
    }
  }
}

void TasksManager::startTouchProcessTask(TaskHandle_t *task)
{
  xTaskCreatePinnedToCore(
      processTouchTask,
      "Screen Process Touch Task",
      8092,
      &context_,
      1,
      task,
      1
  );
}

void TasksManager::updateView(void *parameter)
{
  TasksContext *ctx = (TasksContext*)parameter;
  while(true)
  {
    if(xSemaphoreTake(ctx->mutex, 0))
    {
      if(ctx->data->code != ScreenDataHolder::ExitCodes::Success
      && ctx->data->code != ScreenDataHolder::ExitCodes::Waiting)
        ctx->UI->showNotification(ctx->screenManager->gfx, ctx->data);
      ctx->UI->updateView(ctx->data, ctx->screenManager->gfx);
      if (ctx->data->useButton)
      {
        ctx->UI->useSelectedButton(ctx->screenManager->gfx, ctx->data);
        ctx->data->useButton = false;
      }
      if (ctx->data->switchButton)
      {
        ctx->UI->selectButton(ctx->data->switchButton, ctx->screenManager->gfx, ctx->data);
        ctx->data->switchButton = 0;
      }
      ctx->UI->toggleMotorButton(ctx->data, ctx->screenManager->gfx);
      xSemaphoreGive(ctx->mutex);
    }
    vTaskDelay(200);
  }
}

void TasksManager::startUpdateViewTask()
{
  xTaskCreatePinnedToCore(
      updateView,
      "Screen Update View Task",
      8092,
      &context_,
      2,
      NULL,
      1
  );
}

void TasksManager::updateVoltageData(void *parameter)
{
  TasksContext *ctx = (TasksContext*)parameter;
  while(true)
  {
    xTaskNotifyWait(0, ULONG_MAX, NULL, portMAX_DELAY);
    uint16_t *minVoltages;
    uint16_t *maxVoltages;
    uint16_t minStartIndex;
    uint16_t maxStartIndex;
    uint16_t arraySize;
    ctx->connection->getVoltageData(&minVoltages, &maxVoltages, minStartIndex, maxStartIndex, arraySize);
    ctx->data->updateVoltageData(minVoltages, maxVoltages, minStartIndex, maxStartIndex, arraySize);
  }
}

void TasksManager::startVoltageUpdateTask(TaskHandle_t *task)
{
  xTaskCreatePinnedToCore(
      updateVoltageData,
      "Voltage Update Touch Task",
      8092,
      &context_,
      4,
      task,
      0
  );
}

void TasksManager::updateSignalData(void *parameter)
{
    TasksContext *ctx = (TasksContext*)parameter;
    
    while(true) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        
        uint8_t dataType;
        float value;

        while (ctx->connection->getSignalData(dataType, value)) {
            ctx->data->updateSignalData(dataType, value);
        }
    }
}

void TasksManager::startSignalUpdateTask(TaskHandle_t *task)
{
  xTaskCreatePinnedToCore(
      updateSignalData,
      "Signal Update Task",
      8092,
      &context_,
      4,
      task,
      0
  );
}

void TasksManager::startTasks(TaskHandle_t *screenClickedTask, TaskHandle_t *voltageUpdateTask, TaskHandle_t *signalDataUpdateTask, TaskHandle_t *connectionHandleTask)
{
  startConnectionTask(connectionHandleTask);
  startTouchLoopTask();
  startTouchProcessTask(screenClickedTask);
  startUpdateViewTask();
  startVoltageUpdateTask(voltageUpdateTask);
  startSignalUpdateTask(signalDataUpdateTask);
}
