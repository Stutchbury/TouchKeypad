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
 * DisplayTouchKeypad class.
 * 
 * GPLv2 Licence https://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 * 
 * Copyright (c) 2022 Philip Fletcher <philip.fletcher@stutchbury.com>
 * 
 */

#include "TouchKeypad.h"

TouchKey::TouchKey() : DisplayArea(0,0,0,0) {}

TouchKey::TouchKey(TouchKeypad* keypad, DisplayArea area, uint8_t row, uint8_t col)
      : DisplayArea(area), keypad(keypad), _row(row), _col(col)
    { }


    /**
     * This is the un-touched 'timeout'
     */
    void TouchKey::update() {
      unsigned long now = millis();
      if ( _isTouched && now > _lastTouched + touchDurationMs) {
        _isTouched = false;
        _lastUntouched = now;
      }
    }

    /**
      Called if TSPoint.z is over threshold.
    */
    bool TouchKey::update(Coords_s c) {
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

    void TouchKey::setTouchDurationMs(uint16_t touchDuration /*= 100*/) {
      touchDurationMs = touchDuration;
    }

    void TouchKey::setTouchRepeatMs(uint16_t touchRepeat /*= 700*/) {
      touchRepeatMs = touchRepeat;
    }


    bool TouchKey::fireKeyTouchedCallback() {
      if ( touchCallback != NULL ) { 
        touchCallback(*this);
        return true;
      }
      return false;
    }

    unsigned long TouchKey::lastTouched() {
      return _lastTouched;
    }

    unsigned long TouchKey::lastUntouched() {
      return _lastUntouched;
    }

    DisplayArea TouchKey::getTouchKeyArea() {
      return DisplayArea(x(), y(), w(), h());
    }

    void TouchKey::setTouchedHandler(TouchKeyFunction f) {
      touchCallback = f;
    }

    uint8_t TouchKey::row() {
      return _row;
    }

    uint8_t TouchKey::col() {
      return _col;
    }


    bool TouchKey::enabled() {
      return _enabled;
    }

    void TouchKey::enable(bool e /*=true*/) {
      _enabled = e;
    }

    /**
     * Set the key user identifier
     */
    void TouchKey::setUserId(unsigned int userId) { _userId = userId; }
    
    /**
     * Set the key user state.
     */    
    void TouchKey::setUserState(unsigned int userState) { _userState = userState; }    

    /**
     * Get the key user identifier (not unique, defaults to 0)
     */
    unsigned int TouchKey::userId() { return _userId; }

    /**
     * Get the key user state.
     */    
    unsigned int TouchKey::userState() { return _userState; }    
    
//////////////////////////////////////////////////////////////////////////////////
    TouchKeypad::TouchKeypad(TouchScreen& touchscreen, DisplayArea keypadArea, uint8_t rows, uint8_t cols)
      : DisplayArea(keypadArea), ts(touchscreen) { /*, rows(rows), cols(cols)*/
      initKeys(rows, cols);
    }

    void TouchKeypad::setDisplayArea(DisplayArea area) {
      setX(area.x());
      setY(area.y());
      setW(area.w());
      setH(area.h());
      initKeys(rows, cols);
    }

    void TouchKeypad::setDisplayWidth(uint16_t width) {
      displayWidth = width;
    }

    void TouchKeypad::setDisplayHeight(uint16_t height) {
      displayHeight = height;
    }

    void TouchKeypad::setTsTop(uint16_t i) {
      tsTop = i;
    }

    void TouchKeypad::setTsBottom(uint16_t i) {
      tsBottom = i;
    }

    void TouchKeypad::setTsLeft(uint16_t i) {
      tsLeft = i;
    }

    void TouchKeypad::setTsRight(uint16_t i) {
      tsRight = i;
    }


    void TouchKeypad::rotate() {
      uint8_t i = tsLeft;
      tsLeft = tsBottom;
      tsBottom = tsRight;
      tsRight = tsTop;
      tsTop = i;
    }


    void TouchKeypad::flipX() {
      uint16_t i = tsLeft;
      tsLeft = tsRight;
      tsRight = i;
    }

    void TouchKeypad::flipY() {
      uint16_t i = tsTop;
      tsTop = tsBottom;
      tsBottom = i;
    }


    /**
     * This is the 'abstract' key touch handler method. By default returns
     * true if the TouchKeyCallback has been fired.
     * Override in subclasses.
     */
    bool TouchKeypad::onKeyTouched(TouchKey& tk) {
      return tk.fireKeyTouchedCallback();
    }



    void TouchKeypad::update(uint16_t touchUpdateMs /*= 10*/) {
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
            // Serial.printf("Raw x: %i, Y: %i \n", p.x, p.y);
            // Serial.printf("X: %i, Y: %i\n"
            //             , map(p.x, tsLeft, tsRight, 0, r()) //@TODO can't use right
            //             , map(p.y, tsTop, tsBottom, 0, b()) ///@TODO can't use bottom
            //             );

            coords.x = map(p.x, tsLeft, tsRight, 0, displayWidth);
            coords.y = map(p.y, tsTop, tsBottom, 0, displayHeight);
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

    void TouchKeypad::initKeys(uint8_t rows, uint8_t cols) {
      this->rows = rows;
      this->cols = cols;
      for ( uint8_t r = 0; r < rows; r++ ) {
        for ( uint8_t c = 0; c < cols; c++ ) {
          keys[r][c] = TouchKey(this, DisplayArea(xDiv(cols, c), yDiv(rows, r), wDiv(cols), hDiv(rows)), r, c);
        }
      }
    }

    void TouchKeypad::setTouchDurationMs(uint16_t touchDuration /*= 100*/) {
      for ( uint8_t r = 0; r < rows; r++ ) {
        for ( uint8_t c = 0; c < cols; c++ ) {
          keys[r][c].setTouchDurationMs(touchDuration);
        }
      }
    }

    void TouchKeypad::setTouchRepeatMs(uint16_t touchRepeat /*= 700*/) {
      for ( uint8_t r = 0; r < rows; r++ ) {
        for ( uint8_t c = 0; c < cols; c++ ) {
          keys[r][c].setTouchRepeatMs(touchRepeat);
        }
      }
    }


    TouchKey& TouchKeypad::key(uint8_t row, uint8_t col) {
      return keys[row][col];
    }

    bool TouchKeypad::enabled() {
      return _enabled;
    }

    void TouchKeypad::enable(bool e) {
      _enabled = e;
    }

    /**
     * Set the keypad user identifier
     */
    void TouchKeypad::setUserId(unsigned int userId) { _userId = userId; }
    
    /**
     * Set the keypad user state.
     */    
    void TouchKeypad::setUserState(unsigned int userState) { _userState = userState; }    

    /**
     * Get the keypad user identifier (not unique, defaults to 0)
     */
    unsigned int TouchKeypad::userId() { return _userId; }

    /**
     * Get the keypad user state.
     */    
    unsigned int TouchKeypad::userState() { return _userState; }    

