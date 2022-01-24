/**
 * A touch keypad for Arduino/Teensy etc that extends DisplayUtils Area.
 * https://github.com/Stutchbury/DisplayUtils
 * Requires Adafruit's TouchScreen.h 
 * https://github.com/adafruit/Adafruit_TouchScreen
 * 
 * Two classes - one for each key and one that represents the keypad.
 * 
 * NOTE: You would not normally instantiate either of these classes  as
 * they do not draw anything on the screen! For that you need the 
 * DisplayTouckKeypad class.
 * 
 * NOTE: This is a very early release (for a specific project) so 
 * assumes a screen of 320x240 and a rotation of 1.
 * I will work on sorting that...
 * 
 * 
 */
/*
MIT License

Copyright (c) 2022 Philip Fletcher <philip.fletcher@stutchbury.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice must be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/



#ifndef TOUCH_KEYPAD_H
#define TOUCH_KEYPAD_H


#include "Arduino.h"


#ifndef DISPLAY_UTILS_H
#include <DisplayUtils.h>
#endif


#ifndef _ADAFRUIT_TOUCHSCREEN_H_
#include <TouchScreen.h>
#endif



#ifndef TOUCH_KEYPAD_MAX_ROWS
#define TOUCH_KEYPAD_MAX_ROWS 4
#endif
#ifndef TOUCH_KEYPAD_MAX_COLS
#define TOUCH_KEYPAD_MAX_COLS 5
#endif

#include "Arduino.h"

class TouchKeypad; //Forward declare

/** ******************************************************************************
 *  The class that is the key :-)


*/
class TouchKey : public DisplayArea {

  public:
    typedef void (*TouchKeyFunction) (TouchKey&);

  private:
    TouchKeypad* keypad;
    bool _isTouched = false;
    unsigned long _lastTouched = 0;
    unsigned long _lastUntouched = 0;
    TouchKeyFunction touchCallback = NULL;
    uint16_t touchDurationMs = 100;
    uint16_t touchRepeatMs = 700;
    uint8_t _row = 0;
    uint8_t _col = 0;
    unsigned int _userId = 0;
    unsigned int _userState = 0;
    bool _enabled = true;

  
  public:
    TouchKey() : DisplayArea(0,0,0,0) {}
    TouchKey(TouchKeypad* keypad, DisplayArea area, uint8_t row, uint8_t col)
      : DisplayArea(area), keypad(keypad), _row(row), _col(col)
    { }


    /**
     * This is the un-touched 'timeout'
     */
    void update() {
      unsigned long now = millis();
      if ( _isTouched && now > _lastTouched + touchDurationMs) {
        _isTouched = false;
        _lastUntouched = now;
      }
    }

    /**
      Called if TSPoint.z is over threshold.
    */
    bool update(Coords_s c) {
      unsigned long now = millis();
      update();
      if ( contains(c.x, c.y) ) {
        if (!_isTouched && now > (_lastTouched + touchRepeatMs) ) {
          //Only reset touch if untouched by update() or by different key
          _isTouched = true;
          _lastTouched = now;
          return true;
        }
      } else {
        _isTouched = false; //A different key has been touched        
        _lastUntouched = now;
      }
      return false;
    }

    bool fireKeyTouchedCallback() {
      if ( touchCallback != NULL ) { 
        touchCallback(*this);
        return true;
      }
      return false;
    }

    unsigned long lastTouched() {
      return _lastTouched;
    }

    unsigned long lastUntouched() {
      return _lastUntouched;
    }

    DisplayArea getTouchKeyArea() {
      return DisplayArea(x(), y(), w(), h());
    }

    void setTouchedHandler(TouchKeyFunction f) {
      touchCallback = f;
    }

    uint8_t row() {
      return _row;
    }

    uint8_t col() {
      return _col;
    }

    bool enabled() {
      return _enabled;
    }

    void enable(bool e=true) {
      _enabled = e;
    }

    /**
     * Set the key user identifier
     */
    void setUserId(unsigned int userId) { _userId = userId; }
    
    /**
     * Set the key user state.
     */    
    void setUserState(unsigned int userState) { _userState = userState; }    

    /**
     * Get the key user identifier (not unique, defaults to 0)
     */
    unsigned int userId() { return _userId; }

    /**
     * Get the key user state.
     */    
    unsigned int userState() { return _userState; }    
    
};

/** ******************************************************************************
 *  This is a base class to handle touch and touch callbacks - it won't draw anything
 *  on the display! Use DisplayTouchKeypad to display your
*/
class TouchKeypad : public DisplayArea {


  public:
    /**
       Constructor
    */
    TouchKeypad(TouchScreen& touchscreen, DisplayArea keypadArea, uint8_t rows, uint8_t cols)
      : DisplayArea(keypadArea), ts(touchscreen) { /*, rows(rows), cols(cols)*/
      initKeys(rows, cols);
    }



    /**
     * This is the 'abstract' key touch handler method. By default returns
     * true if the TouchKeyCallback has been fired.
     * Override in subclasses.
     */
    virtual bool onKeyTouched(TouchKey& tk) {
      return tk.fireKeyTouchedCallback();
    }



    void update(uint16_t touchUpdateMs = 10) {
      if ( _enabled ) {
        now = millis();
        if ( now > lastTouchUpdate + touchUpdateMs ) {
          lastTouchUpdate = now;
          //Get the TSPoint
          TSPoint p = ts.getPoint();
          bool haveTouch = false;
          Coords_s coords = {0, 0};
          if (p.z > ts.pressureThreshhold) {
            haveTouch = true;
            //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
            //@TODO This will have to change for screen rotation...
            coords.x = displayWidth - map(p.x, tsMinX, tsMaxX, 0, displayWidth);
            coords.y = map(p.y, tsMinY, tsMaxY, 0, displayHeight);
            //coords = mapTSPoint(p);
            //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
            //Serial.printf("coords c: %i, y: %i \n", coords.x, coords.y);
          }
          for ( uint8_t r = 0; r < rows; r++ ) {
            for ( uint8_t c = 0; c < cols; c++ ) {
              //Update each key with Coords_s
              if ( haveTouch && keys[r][c].update(coords) ) {
                onKeyTouched(keys[r][c]);
              } else {
                keys[r][c].update();
              }
            }
          }
        }
      }
    }

    void initKeys(uint8_t rows, uint8_t cols) {
      this->rows = rows;
      this->cols = cols;
      for ( uint8_t r = 0; r < rows; r++ ) {
        for ( uint8_t c = 0; c < cols; c++ ) {
          keys[r][c] = TouchKey(this, DisplayArea(xDiv(cols, c), yDiv(rows, r), wDiv(cols), hDiv(rows)), r, c);
        }
      }
    }


    TouchKey& key(uint8_t row, uint8_t col) {
      return keys[row][col];
    }

    bool enabled() {
      return _enabled;
    }

    void enable(bool e) {
      _enabled = e;
    }

    /**
     * Set the keypad user identifier
     */
    void setUserId(unsigned int userId) { _userId = userId; }
    
    /**
     * Set the keypad user state.
     */    
    void setUserState(unsigned int userState) { _userState = userState; }    

    /**
     * Get the keypad user identifier (not unique, defaults to 0)
     */
    unsigned int userId() { return _userId; }

    /**
     * Get the keypad user state.
     */    
    unsigned int userState() { return _userState; }    

  protected:
    TouchScreen& ts;
    uint8_t rows = 0;
    uint8_t cols = 0;
    bool _enabled = true;
    TouchKey keys[TOUCH_KEYPAD_MAX_ROWS][TOUCH_KEYPAD_MAX_COLS];
    uint16_t displayWidth = 320;
    uint16_t displayHeight = 240;
    //@TODO setTouchBounds()
    uint16_t tsMinX = 109;
    uint16_t tsMinY = 110;
    uint16_t tsMaxX = 940;
    uint16_t tsMaxY = 945;
    unsigned long now = millis();
    //uint16_t touchUpdateMs = 10;
    unsigned long lastTouchUpdate = 0;
    unsigned int _userId = 0;
    unsigned int _userState = 0;


    /**
       @TODO we may have to mess with this for screen rotation...
    */
    Coords_s mapTSPoint(
      TSPoint p,
      uint16_t displayWidth,
      uint16_t displayHeight,
      uint16_t tsMinX,
      uint16_t tsMinY,
      uint16_t tsMaxX,
      uint16_t tsMaxY
    ) {
      return //Coords_s(
      { (displayWidth - map(p.x, tsMinX, tsMaxX, 0, displayWidth)),
        (map(p.y, tsMinY, tsMaxY, 0, displayHeight))
      };
      //);
    }
    /**
      @TODO
    */
    Coords_s mapTSPoint( TSPoint p ) {
      return mapTSPoint(p, displayWidth, displayHeight, tsMinX, tsMaxX, tsMinY, tsMaxY);
    }



};



#ifdef _ADAFRUIT_GFX_H
#include "gfx/DisplayTouchKeypad.h"
#endif



#endif
