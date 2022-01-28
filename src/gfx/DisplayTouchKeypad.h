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

#ifndef TOUCH_KEYPAD_H
#include "TouchKeypad.h"
#endif

/**
 * This class actually draws stuff on the display.
 */
class DisplayTouchKeypad : public TouchKeypad {

  public:
    //typedef void (*TouchKeyDrawFunction) (DisplayArea&);
    typedef void (*TouchKeyDrawFunction) (TouchKey&);

  
  protected:
    char labels[TOUCH_KEYPAD_MAX_ROWS][TOUCH_KEYPAD_MAX_COLS][5];
    Adafruit_GFX& gfx;
    const GFXfont* font;
    uint16_t fgColour = 65535; //white
    uint16_t bgColour = 0; //black
    uint16_t touchedColour = 65535; //white

  
  private:

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


  public:
    /**
       Constructor with no font set - either setFont() or al buttons use draw function
    */
    DisplayTouchKeypad(Adafruit_GFX& d, TouchScreen& touchscreen, DisplayArea keypadArea, uint8_t rows, uint8_t cols)
      : TouchKeypad(touchscreen, keypadArea, rows, cols), gfx(d) {
    }

    /**
       Constructor with font set
    */
    DisplayTouchKeypad(Adafruit_GFX& d, const GFXfont* f, TouchScreen& touchscreen, DisplayArea keypadArea, uint8_t rows, uint8_t cols)
      : TouchKeypad(touchscreen, keypadArea, rows, cols), gfx(d), font(f) {
    }


    /**
     * Call both update and draw
     * 
     * If overriding eith update or draw in supcallses, you must override this too (if
     * you're going to call it) 
     * 
     * @param displayRefreshMs 
     * @param touchUpdateMs 
     */
    void updateAndDraw(uint16_t touchUpdateMs = 10, uint16_t displayRefreshMs=100)  {
      TouchKeypad::update(touchUpdateMs);
      draw(displayRefreshMs);
    }

    void draw(uint16_t displayRefreshMs=100) {
      if ( _enabled ) {
        //Do event updates and fire touch handlers
        now = millis();
        //redraw as required
        if ( now > lastDisplayRefresh + displayRefreshMs ) {
          lastDisplayRefresh = now;
          if ( forceRefresh ) {
            gfx.fillRect(x(), y(), w(), h(), bgColour);
          }
          int16_t  x, y;
          uint16_t w, h;
          for ( uint8_t row = 0; row < rows; row++ ) {
            for ( uint8_t col = 0; col < cols; col++ ) {
              if ( key(row, col).enabled() ) {
                bool keyStale = ( drawnUserId[row][col] != key(row, col).userId() || drawnUserState[row][col] != key(row, col).userState() );
                drawnUserId[row][col] = key(row, col).userId();
                drawnUserState[row][col] = key(row, col).userState();
                //Draw the feedback flash
                if ( key(row, col).lastTouched() > key(row, col).lastUntouched() ) {
                  //if ( key(row, col).enabled() ) {
                    gfx.fillRoundRect(key(row, col).x()+keyMargin, key(row, col).y()+keyMargin, key(row, col).w()-(keyMargin*2), key(row, col).h()-(keyMargin*2), keyRadius, touchedColour);
                  //}
                  keyUntouched[row][col] = false;
                } else if ( forceRefresh || keyStale || (!keyUntouched[row][col] && now > untouchDelayMs + key(row, col).lastTouched() ) ) {
                  gfx.fillRect(key(row, col).x(), key(row, col).y(), key(row, col).w(), key(row, col).h(), bgColour);
                  if ( keyLabelFunctions[row][col] != NULL ) {
                    keyLabelFunctions[row][col](keys[row][col]);
                  } else {
                    gfx.drawRoundRect(key(row, col).x()+keyMargin, key(row, col).y()+keyMargin, key(row, col).w()-(keyMargin*2), key(row, col).h()-(keyMargin*2), keyRadius, fgColour);
                    if ( strlen(labels[row][col]) > 0 ) {
                      gfx.setFont(font);
                      gfx.setTextColor(fgColour);
                      gfx.getTextBounds(labels[row][col], key(row, col).x(), key(row, col).b(), &x, &y, &w, &h);
                      gfx.setCursor(key(row, col).xCl() - (w / 2), key(row, col).yCl() + (h / 2));
                      gfx.print(labels[row][col]);
                    }
                  }
                  keyUntouched[row][col] = true;
                }
              }
            }
          }
          forceRefresh = false;
        }
      }
    }


    void setLabel(uint8_t row, uint8_t col, char label[5]) {
      strcpy(labels[row][col], label);
    }

    char* getLabel(uint8_t row, uint8_t col) {
      return labels[row][col];
    }

    void setDrawHandler(uint8_t row, uint8_t col, TouchKeyDrawFunction f) {
      keyLabelFunctions[row][col] = f;
    }

    void setFont(const GFXfont* f) {
      if (font == NULL || font != f) {
        font = f;
        //gfx.setFont(font);
      }
    }

    void enable(bool b=true) {
      TouchKeypad::enable(b);
      gfx.fillRect(x(), y(), w(), h(), bgColour);
      forceRefresh = b;
    }
      
};


#endif