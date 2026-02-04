#include "UserInterface.h"

void UserInterface::processClick(const int16_t &x, const int16_t &y, Arduino_RGB_Display *gfx, ScreenDataHolder *data)
{
  processingClick_ = true;
  for(size_t index = 0; index < currentView->numButtons; ++index)
  {
    if(currentView->isWithinBounds(x, y, index))
    {
      processButton(gfx, data, index);
      index = currentView->numButtons;
    }
  }
  processingClick_ = false;
}

void UserInterface::processButton(Arduino_RGB_Display *gfx, ScreenDataHolder *data, const size_t &index)
{
  currentView->callFunctionPointer(index, data, gfx, currentView->getButtonText(index, data->language), index);
  View* newView = currentView->getLinkedView(index);
  if (newView)
    changeView(gfx, newView, data);
}

void UserInterface::changeView(Arduino_RGB_Display *gfx, View *newView, ScreenDataHolder *data)
{
  currentView = newView;
  currentView->showView(gfx, data);
  data->selectedButton = 0;
}

void UserInterface::initializeUI(View **_views, Arduino_RGB_Display *gfx, ScreenDataHolder *data)
{
  views_ = _views;
  currentView = views_[0];
  currentView->showView(gfx, data);
}

void UserInterface::selectButton(const int8_t &direction, Arduino_RGB_Display *gfx, ScreenDataHolder *data)
{
  uint8_t previousSelected = data->selectedButton;
  data->selectedButton = (data->selectedButton + direction + currentView->numButtons) % currentView->numButtons;
  currentView->selectButton(previousSelected, data->selectedButton, gfx, data);
}

void UserInterface::toggleMotorButton(ScreenDataHolder *data, Arduino_RGB_Display *gfx)
{
  static const uint16_t color = getButtonColor(2);
  static bool toggled = true;
  if (data->operationNotDisplaying)
    return;
  if(data->operating && toggled)
  {
    updateButtonColor(2, RGB565_DARKGREY);
    updateButtonColor(3, RGB565_DARKGREY);
    showButton(2, gfx, data);
    showButton(3, gfx, data);
    toggled = false;
  }
  else if(!data->operating && !toggled)
  {
    updateButtonColor(2, color);
    updateButtonColor(3, color);
    showButton(2, gfx, data);
    showButton(3, gfx, data);
    toggled = true;
  }
}
