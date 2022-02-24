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
 * GPLv2 Licence https://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 * 
 * Copyright (c) 2022 Philip Fletcher <philip.fletcher@stutchbury.com>
 * 
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

#if defined(ARDUINO_ARCH_ESP32) || defined(ESP8266) || defined(CORE_TEENSY)
  #include <functional>
#endif

class TouchKeypad; //Forward declare

/** ******************************************************************************
 *  The class that is the key :-)
*/
class TouchKey : public DisplayArea {

  public:
    #if defined(ARDUINO_ARCH_ESP32) || defined(ESP8266) || defined(CORE_TEENSY)
      typedef std::function<void(TouchKey &btn)> TouchKeyFunction;
    #else
    typedef void (*TouchKeyFunction) (TouchKey&);
    #endif    

  
    /**
     * @brief Construct a new Touch Key object
     * 
     */
    TouchKey(TouchKeypad* keypad, DisplayArea area, uint8_t row, uint8_t col);
    TouchKey();

    /**
     * @brief Called by TouchKeypad in the loop/update() 
     * for each key. This is will 'un-touched' the key
     */
    void update();

    /**
     * @brief Called by TouchKeypad in the loop/update()
     * for each key only if TSPoint.z is over threshold.
     */
    bool update(Coords_s c);

    /**
     * @brief Set touchDurationMs - normally set only via TouchKeypad
     * 
     * @param touchDurationMs 
     */
    void setTouchDurationMs(uint16_t touchDurationMs = 100);

    /**
     * @brief Set touchRepeatMs - normally set via TouchKeypad
     * 
     * @param touchRepeatMs 
     */
    void setTouchRepeatMs(uint16_t touchRepeatMs = 700);

    /**
     * @brief Fire the 'touched' event callback function/method
     * for this key if it has been set.
     * 
     * @return true 
     * @return false 
     */

    bool fireKeyTouchedCallback();

    /**
     * @brief Return the ms this key was last touched
     * 
     * @return unsigned long 
     */
    unsigned long lastTouched();

    /**
     * @brief Return the ms this key was last 'untouched'
     * 
     * @return unsigned long 
     */
    unsigned long lastUntouched();

    /**
     * @brief Return the TouchKey DisplayArea
     * 
     * @return DisplayArea 
     */
    DisplayArea getTouchKeyArea();

    /**
     * @brief Set the Touched Handler for this key
     * 
     * @param TouchKeyFunction
     */
    void setTouchedHandler(TouchKeyFunction f);

    /**
     * @brief Get the row assigned to the key
     * 
     * @return uint8_t 
     */
    uint8_t row();

    /**
     * @brief Get the column assigned to the key 
     * 
     * @return uint8_t 
     */
    uint8_t col();

    bool enabled();

    void enable(bool e=true);

    /**
     * Set the key user identifier
     */
    void setUserId(unsigned int userId);

    /**
     * Set the key user state.
     */    
    void setUserState(unsigned int userState);

    /**
     * Get the key user identifier (not unique, defaults to 0)
     */
    unsigned int userId();

    /**
     * Get the key user state.
     */    
    unsigned int userState();

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



};

/** **************************************************************************** */
/** ******************************************************************************
 *  This is a base class to handle touch and touch callbacks - it won't draw anything
 *  on the display! Use DisplayTouchKeypad to display your
*/
class TouchKeypad : public DisplayArea {


  public:
    /**
     * Constructor
     */
    TouchKeypad(TouchScreen& touchscreen, DisplayArea keypadArea, uint8_t rows, uint8_t cols);



    void setDisplayArea(DisplayArea area);

    void setDisplayWidth(uint16_t width);
    void setDisplayHeight(uint16_t height);
    void setTsTop(uint16_t i);
    void setTsBottom(uint16_t i);
    void setTsLeft(uint16_t i);
    void setTsRight(uint16_t i);

    void update(uint16_t touchUpdateMs = 10);

    /**
     * @brief Unlike the GFX display, this does not take any arguments, it 
     * just rotates the touch screen 90 degrees clockwise.
     * The touchscreen is highly likely to at a completetely different 
     * native rotation to the actual (display) screen and also be (internally) 
     * wired back-to-front in x and/or y.
     * To deal with this we have flipX() & flipY() as below.
     * If required, call rotate() just once and then use flipX() and/or 
     * flipY() to match the display.  
     */
    void rotate();

    /**
     * @brief Swap tsLeft & tsRight boundaries
     */
    void flipX();

    /**
     * @brief Swap tsTop & tsBottom boundaries
     */
    void flipY();    

    /**
     * @brief Initialise the keypad with the number of required
     * rows and columns. Normally done in the constructor but can 
     * be reset at runtime.
     * 
     * @param rows 
     * @param cols 
     */
    void initKeys(uint8_t rows, uint8_t cols);

    /**
     * @brief Set how long the key stays 'touched'
     * 
     * @param touchDurationMs 
     */
    void setTouchDurationMs(uint16_t touchDurationMs = 100);
    
    /**
     * @brief Set how frequently the touch event is fired 
     * if the key is continuously pressed
     *  
     * @param touchRepeatMs 
     */
    void setTouchRepeatMs(uint16_t touchRepeatMs = 700);



    /**
     * @brief This is the 'abstract' key touch handler method. By default returns
     * true if the TouchKeyCallback has been fired.
     * Override in subclasses.
     * 
     * @param tk 
     * @return true 
     * @return false 
     */
    virtual bool onKeyTouched(TouchKey& tk);

    /**
     * @brief Get a TouchKey based on its row/column position
     * 
     * @param row 
     * @param col 
     * @return TouchKey& 
     */
    TouchKey& key(uint8_t row, uint8_t col);

    /**
     * @brief Returns true if enabled
     * 
     * @return true 
     * @return false 
     */
    bool enabled();

    /**
     * @brief Pass true to enable, false to disable.
     * 
     * @param e 
     */
    void enable(bool e = true);

    /**
     * Set the keypad user identifier
     */
    void setUserId(unsigned int userId);
    
    /**
     * Set the keypad user state.
     */    
    void setUserState(unsigned int userState);

    /**
     * Get the keypad user identifier (not unique, defaults to 0)
     */
    unsigned int userId();

    /**
     * Get the keypad user state.
     */    
    unsigned int userState();

  protected:

    enum TouchscreenEdge { TS_TOP, TS_RIGHT, TS_BOTTOM, TS_LEFT };
    
    TouchScreen& ts;
    uint8_t rows = 0;
    uint8_t cols = 0;
    bool _enabled = true;
    TouchKey keys[TOUCH_KEYPAD_MAX_ROWS][TOUCH_KEYPAD_MAX_COLS];
    uint16_t displayWidth = 320;
    uint16_t displayHeight = 240;
    //@TODO setTouchBounds()

    /**
     * These four members define the active area (boundaries) of 
     * the touchscreen.
     * We swap these values around with rotate or flipX, flipY
     * @TODO store and read these from EEPROM
     */
    uint16_t tsRight = 940;
    uint16_t tsTop = 110;
    uint16_t tsBottom = 945;
    uint16_t tsLeft = 109;
    
    unsigned long now = millis();
    unsigned long lastTouchUpdate = 0;
    unsigned int _userId = 0;
    unsigned int _userState = 0;


};

#ifdef _ADAFRUIT_GFX_H
#include "gfx/DisplayTouchKeypad.h"
#endif

#endif
