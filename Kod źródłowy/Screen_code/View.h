#pragma once

#include <Arduino_GFX_Library.h>
#include "ScreenDataHolder.h"

#define SELECTED_BUTTON_COLOR RGB565_GREEN
#define SELECTED_TEXT_COLOR RGB565_BLACK

class View
{
public:
    struct StaticButtonData {
        int x, y, w, h;
        const char* texts[2];
        uint16_t textColor;
        uint16_t buttonColor;
    };

    size_t numButtons = 1;

    View(const size_t &_numButtons, const uint8_t &_textSize, const uint16_t &_backgroundColor);
    ~View();
    View* getLinkedView(const size_t &buttonIndex);
    uint16_t getButtonColor(const size_t &buttonIndex);
    const char *getButtonText(const size_t &buttonIndex, ScreenDataHolder::Languages language);
    bool isWithinBounds(const int16_t &x, const int16_t &y, const size_t &buttonIndex);
    void setLinkedView(const size_t &index, View* view);
    void setBackgroudColor(const uint16_t &color);
    void setButtonColor(const size_t &buttonIndex, const uint16_t &color);
    void createButton(const size_t &index, StaticButtonData* staticData);
    void showButton(const size_t &index, Arduino_RGB_Display *gfx, ScreenDataHolder *data);
    void selectButton(uint8_t oldIndex, uint8_t index, Arduino_RGB_Display *gfx, ScreenDataHolder *data);
    void showView(Arduino_RGB_Display *gfx, ScreenDataHolder *data);
    static void getTextAlignment(Arduino_RGB_Display *gfx, const int &x, const int &y, const int &width,
                                 const int &height, const char *text, int &alignX, int &alignY);

    template<typename... Args>
    using FunctionPtrType = void (*)(Args...);

    template<typename... Args>
    void setFunctionPointer(const uint8_t &index, FunctionPtrType<Args...> ptr)
    {
      if (index < numButtons)
          buttons_[index].functionPointer_ = reinterpret_cast<void*>(ptr);
      else if(index == UINT8_MAX)
        functionPointer_ = reinterpret_cast<void*>(ptr);
    }

    template<typename... Args>
    void callFunctionPointer(const uint8_t &index, Args... args)
    {
      if (index < numButtons && buttons_[index].functionPointer_)
          reinterpret_cast<void(*)(Args...)>(buttons_[index].functionPointer_)(args...);
      else if(index == UINT8_MAX && functionPointer_)
        reinterpret_cast<void(*)(Args...)>(functionPointer_)(args...);
    }

private:
    struct Button
    {
      View *linkedView = nullptr;
      void* functionPointer_ = nullptr;
      StaticButtonData* staticData = nullptr;

      bool isWithinBounds(const int16_t &x, const int16_t &y)
      {
          return (x >= staticData->x && x <= staticData->x + staticData->w
                  && y >= staticData->y && y <= staticData->y + staticData->h);
      }
      void showButton(Arduino_RGB_Display *gfx, ScreenDataHolder *data, uint16_t buttonColor, uint16_t textColor);
      void showButton(Arduino_RGB_Display *gfx, ScreenDataHolder *data);
      void displayButton(Arduino_RGB_Display *gfx, ScreenDataHolder::Languages language, uint16_t buttonColor, uint16_t textColor) ;
    };

    Button* buttons_;
    uint16_t backgroundColor_ = RGB565_BLACK;
    uint8_t textSize_ = 1;
    void* functionPointer_ = nullptr;
};
