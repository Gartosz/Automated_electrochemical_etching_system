#include "TasksManager.h"
#include "DataManager.h"
#include "DefineViews.h"

ScreenManager screenManager;
UserInterface UI;
ScreenConnectionController connection;
ScreenDataHolder data(connection.BUFFER_ARRAY_SIZE, &connection);
DataManager manager;
TasksManager tasksManager(&screenManager, &UI, &connection, &data);

size_t lastPrint = 0;
TaskHandle_t screenClickedTask = NULL;
TaskHandle_t voltageUpdateTask = NULL;
TaskHandle_t signalDataUpdateTask = NULL;
TaskHandle_t connectionHandleTask = NULL;

void setup()
{
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println("Start!");
  screenManager.init(&screenClickedTask);
  size_t viewsCount;
  UI.initializeUI(makeViews(viewsCount), screenManager.gfx, &data);
  connection.begin(&voltageUpdateTask, &signalDataUpdateTask, &connectionHandleTask, 9600);
  tasksManager.startTasks(&screenClickedTask, &voltageUpdateTask, &signalDataUpdateTask, &connectionHandleTask);
  data.loadParameters();
}

void loop()
{
  unsigned long currentMs = millis();
  data.updateData();
  if (data.operating)
    manager.handleOperation(data, currentMs);
  else if ((data.code != ScreenDataHolder::ExitCodes::Waiting)
           | (data.code != ScreenDataHolder::ExitCodes::Success))
    manager.endSettings(data);
}
