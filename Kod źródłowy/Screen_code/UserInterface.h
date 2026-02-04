#pragma once

#include "View.h"

class UserInterface
{
  public:
  View *currentView;
  ~UserInterface()
  {
    delete[] views_;
  }
  void processClick(const int16_t &x, const int16_t &y, Arduino_RGB_Display *gfx, ScreenDataHolder *data);
  void initializeUI(View **_views, Arduino_RGB_Display *gfx, ScreenDataHolder *data);
  void selectButton(const int8_t &increment, Arduino_RGB_Display *gfx, ScreenDataHolder *data);
  void toggleMotorButton(ScreenDataHolder *data, Arduino_RGB_Display *gfx);

  template<typename... Args>
  void updateView(Args... args)
  {
    if (!processingClick_)
      currentView->callFunctionPointer(-1, args...);
  }

  void showNotification(Arduino_RGB_Display *gfx, ScreenDataHolder *data)
  {
    changeView(gfx, views_[3], data);
  }

  void useSelectedButton(Arduino_RGB_Display *gfx, ScreenDataHolder *data)
  {
    processButton(gfx, data, data->selectedButton);
  }

  private:
  View **views_;
  bool processingClick_ = false;
  void processButton(Arduino_RGB_Display *gfx, ScreenDataHolder *data, const size_t &index);
  void changeView(Arduino_RGB_Display *gfx, View *newView, ScreenDataHolder *data);

  uint16_t getButtonColor(const size_t &index)
  {
    return currentView->getButtonColor(index);
  }

  void updateButtonColor(const size_t &index, const uint16_t &color)
  {
    currentView->setButtonColor(index, color);
  }

  void showButton(const size_t &index, Arduino_RGB_Display *gfx, ScreenDataHolder *data)
  {
    currentView->showButton(index, gfx, data);
  }
};
