#include "View.h"

View::View(const size_t &_numButtons, const uint8_t &_textSize, const uint16_t &_backgroundColor) : numButtons(_numButtons), textSize_(_textSize), backgroundColor_(_backgroundColor)
{
  buttons_ = new Button[numButtons];
}

View::~View() {
  delete[] buttons_;
}

void View::setBackgroudColor(const uint16_t &color)
{
  backgroundColor_ = color;
}

void View::createButton(const size_t &index, StaticButtonData* staticData)
{
  buttons_[index].staticData = staticData;
}

void View::setLinkedView(const size_t &index, View* view)
{
  buttons_[index].linkedView = view;
}

void View::showView(Arduino_RGB_Display *gfx, ScreenDataHolder *data)
{
  gfx->fillScreen(backgroundColor_);
  gfx->setTextSize(textSize_);
  data->setTextSize(textSize_);
  buttons_[0].showButton(gfx, data, SELECTED_BUTTON_COLOR, RGB565_BLACK);
  for (size_t index = 1; index < numButtons; ++index)
    buttons_[index].showButton(gfx, data);
}

void View::Button::showButton(Arduino_RGB_Display *gfx, ScreenDataHolder *data, uint16_t buttonColor, uint16_t textColor) 
{
  data->setTextColor(textColor);
  data->setBackgroundTextColor(buttonColor);
  displayButton(gfx, data->language, buttonColor, textColor);
}

void View::Button::showButton(Arduino_RGB_Display *gfx, ScreenDataHolder *data) 
{
  data->setTextColor(staticData->textColor);
  data->setBackgroundTextColor(staticData->buttonColor);
  displayButton(gfx, data->language, staticData->buttonColor, staticData->textColor);
}

void View::Button::displayButton(Arduino_RGB_Display *gfx, ScreenDataHolder::Languages language, uint16_t buttonColor, uint16_t textColor) 
{
  const char* text = staticData->texts[static_cast<uint8_t>(language)];
  
  gfx->fillRect(staticData->x, staticData->y, staticData->w, staticData->h, buttonColor);
  gfx->setTextColor(textColor, buttonColor);
  
  int textBegin[2];
  getTextAlignment(gfx, staticData->x, staticData->y, staticData->w, staticData->h, 
                   text, textBegin[0], textBegin[1]);
  
  gfx->setCursor(textBegin[0], textBegin[1]);
  gfx->print(text);
}

void View::getTextAlignment(Arduino_RGB_Display *gfx, const int &x, const int &y, const int &width, const int &height, const char *text, int &alignX, int &alignY)
{
  int16_t x1, y1;
  uint16_t w, h;
  gfx->getTextBounds(text, x, y, &x1, &y1, &w, &h);
  alignX = x + (width - w) / 2;
  alignY = y + height - (height - h) / 2;
}

bool View::isWithinBounds(const int16_t &x, const int16_t &y, const size_t &buttonIndex)
{
  if (buttonIndex >= numButtons)
    return false;
  return buttons_[buttonIndex].isWithinBounds(x, y);
}

const char *View::getButtonText(const size_t &buttonIndex, ScreenDataHolder::Languages language)
{
    if (buttonIndex >= numButtons || !buttons_[buttonIndex].staticData)
        return "";
    
    return buttons_[buttonIndex].staticData->texts[static_cast<uint8_t>(language)];
}

View* View::getLinkedView(const size_t &buttonIndex)
{
  if (buttonIndex >= numButtons)
    return nullptr;
  return buttons_[buttonIndex].linkedView;
}

void View::setButtonColor(const size_t &buttonIndex, const uint16_t &color)
{
  if (buttonIndex < numButtons)
      buttons_[buttonIndex].staticData->buttonColor = color;
}

uint16_t View::getButtonColor(const size_t &buttonIndex)
{ 
  if (buttonIndex >= numButtons)
      return 0;
  return buttons_[buttonIndex].staticData->buttonColor;
}

void View::showButton(const size_t &index, Arduino_RGB_Display *gfx, ScreenDataHolder *data)
{
  buttons_[index].showButton(gfx, data);
}

void View::selectButton(uint8_t oldIndex, uint8_t index, Arduino_RGB_Display *gfx, ScreenDataHolder *data)
{
  buttons_[oldIndex].showButton(gfx, data);
  buttons_[index].displayButton(gfx, data->language, SELECTED_BUTTON_COLOR, RGB565_BLACK);
}

