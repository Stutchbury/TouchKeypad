/**
 * A touch keypad for Arduino/Teensy etc.
 * Requires Adafruit's TouchScreen.h and GFX library (or rather, a
 * subclass of it for your display)
 * https://github.com/adafruit/Adafruit_TouchScreen
 * https://github.com/adafruit/Adafruit-GFX-Library
 * 
 * This class actually draws things on the screen. See example(s) for
 * implementation details.
 * 
 * GPLv2 Licence https://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 * 
 * Copyright (c) 2022 Philip Fletcher <philip.fletcher@stutchbury.com>
 * 
 */


#ifndef DISPLAY_TOUCH_KEYPAD_H
#define DISPLAY_TOUCH_KEYPAD_H

#include "Adafruit_GFX.h"

#include "TouchKeypad.h"

#if defined(ARDUINO_ARCH_ESP32) || defined(ESP8266) || defined(CORE_TEENSY)
  #include <functional>
#endif

/**
 * This class actually draws stuff on the display.
 */
class DisplayTouchKeypad : public TouchKeypad {

  public:
    #if defined(ARDUINO_ARCH_ESP32) || defined(ESP8266) || defined(CORE_TEENSY)
      typedef std::function<void(TouchKey &btn)> TouchKeyDrawFunction;
    #else
      typedef void (*TouchKeyDrawFunction) (TouchKey&);
    #endif    

  
    /**
     * Constructor with no font set - either setFont() or all buttons use a draw function
     */
    DisplayTouchKeypad(Adafruit_GFX& d, TouchScreen& touchscreen, DisplayArea keypadArea, uint8_t rows, uint8_t cols);

    /**
     * Constructor with a font set
     */
    DisplayTouchKeypad(Adafruit_GFX& d, const GFXfont* f, TouchScreen& touchscreen, DisplayArea keypadArea, uint8_t rows, uint8_t cols);


    /**
     * Call both update and draw
     * 
     * If overriding either update or draw in subclasses, you must override this too (if
     * you're going to call it) 
     * 
     * @param displayRefreshMs 
     * @param touchUpdateMs 
     */
    void updateAndDraw(uint16_t touchUpdateMs = 10, uint16_t displayRefreshMs=100);

    /**
     * @brief Draw the keys and refresh the touched UI feedback
     * 
     * @param displayRefreshMs - the refresh rate to update the display
     */
    void draw(uint16_t displayRefreshMs=100);

    /**
     * @brief Set a label for a specific key. Max 4 chars (plus null)
     * 
     * @param row 
     * @param col 
     * @param label 
     */
    void setLabel(uint8_t row, uint8_t col, char label[5]);

    /**
     * @brief Set the Background Colour of the keys
     * 
     * @param colour 
     */
    void setBackgroundColour(uint16_t colour);

    /**
     * @brief Set the Foreground Colour of the keys
     * 
     * @param colour 
     */
    void setForegroundColour(uint16_t colour);

    /**
     * @brief Set the that flashes on touch
     * 
     * @param colour 
     */
    void setTouchedColour(uint16_t colour);


    /**
     * @brief Get the Label for a key
     * 
     * @param row 
     * @param col 
     * @return char* 
     */
    char* getLabel(uint8_t row, uint8_t col);

    /**
     * @brief Set a Draw Handler for a a key - if set this will be used
     * rather than drawing a label.
     * 
     * @param row 
     * @param col 
     * @param f 
     */
    void setDrawHandler(uint8_t row, uint8_t col, TouchKeyDrawFunction f);

    /**
     * @brief Set the Font for text labels
     * 
     * @param f 
     */
    void setFont(const GFXfont* f);

    /**
     * @brief Set the DisplayArea of the DisplayTouchKeypad to the
     * background colour.
     * 
     */
    void clear();

  protected:
    char labels[TOUCH_KEYPAD_MAX_ROWS][TOUCH_KEYPAD_MAX_COLS][5];
    Adafruit_GFX& gfx;
    const GFXfont* font;
    uint16_t fgColour = 65535; //white
    uint16_t bgColour = 0; //black
    uint16_t touchedColour = 65535; //white
  
    TouchKeyDrawFunction keyLabelFunctions[TOUCH_KEYPAD_MAX_ROWS][TOUCH_KEYPAD_MAX_COLS];
    unsigned int drawnUserId[TOUCH_KEYPAD_MAX_ROWS][TOUCH_KEYPAD_MAX_COLS];
    unsigned int drawnUserState[TOUCH_KEYPAD_MAX_ROWS][TOUCH_KEYPAD_MAX_COLS];

    //uint16_t displayRefreshMs = 100;
    unsigned long lastDisplayRefresh = 0;

    bool keyUntouched[TOUCH_KEYPAD_MAX_ROWS][TOUCH_KEYPAD_MAX_COLS];
    const uint16_t untouchDelayMs = 100;
    uint8_t keyMargin = 2;
    uint8_t keyRadius = 4;

    bool forceRefresh = false;



};


#endif
