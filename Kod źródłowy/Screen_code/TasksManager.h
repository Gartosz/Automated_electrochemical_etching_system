#pragma once

#include "ScreenManager.h"

class TasksManager
{
  public:
    TasksManager(ScreenManager *screenManager, UserInterface *UI, ScreenConnectionController *connection, ScreenDataHolder *data) :
    context_{screenManager, UI, connection, data, xSemaphoreCreateMutex()} {}
    void startTasks(TaskHandle_t *screenClickedTask, TaskHandle_t *voltageUpdateTask, TaskHandle_t *signalDataUpdateTask, TaskHandle_t *connectionHandleTask);

  private:
  typedef struct TasksContext
    {
      ScreenManager *screenManager;
      UserInterface *UI;
      ScreenConnectionController *connection;
      ScreenDataHolder *data;
      SemaphoreHandle_t mutex;
    };
    TasksContext context_;

    static void triggerTask(void *parameter);
    static void checkTouchTask(void *parameter);
    static void processTouchTask(void *parameter);
    static void updateView(void *parameter);
    static void updateVoltageData(void *parameter);
    static void updateSignalData(void *parameter);
    void startConnectionTask(TaskHandle_t *task);
    void startTouchLoopTask();
    void startTouchProcessTask(TaskHandle_t *task);
    void startUpdateViewTask();
    void startVoltageUpdateTask(TaskHandle_t *task);
    void startSignalUpdateTask(TaskHandle_t *task);
};

