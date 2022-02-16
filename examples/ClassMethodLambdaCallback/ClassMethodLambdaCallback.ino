/**
 * A bare minimum example touch keypad for Arduino/Teensy etc.
 * No button labels are drawn in this ezample but the touch event
 * (in this example, common to all keys) will write to Serial.
 * 
 * Requires Adafruit's TouchScreen.h and GFX library (or rather, a
 * subclass of it for your display)
 * https://github.com/adafruit/Adafruit_TouchScreen
 * https://github.com/adafruit/Adafruit-GFX-Library
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

#include "SPI.h"
#include "Adafruit_GFX.h"

//Use the GFX sub class appropriate for your display
#include "Adafruit_ILI9341.h"


#include <TouchKeypad.h>

// For the Adafruit shield, these are the default.
#define TFT_DC 9
#define TFT_CS 10

//Use the GFX sub class appropriate for your display
// Use hardware SPI (on Uno, #13, #12, #11) and the above for CS/DC
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
// If using the breakout, change pins as desired
//Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);

//UNO
//#define YP A2  // must be an analog pin, use "An" notation!
//#define XM A3  // must be an analog pin, use "An" notation!
//#define YM 8   // can be a digital pin
//#define XP 9   // can be a digital pin

//My Teensy 4.1
#define YP A6  // X- must be an analog pin, use "An" notation!
#define XM A9  // Y+ must be an analog pin, use "An" notation!
#define YM A8   // X+ can be a digital pin
#define XP A7   // Y- can be a digital pin

// For better pressure precision, we need to know the resistance
// between X+ and X- Use any multimeter to read it
// For the one we're using, its 300 ohms across the X plate
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);


const uint8_t rows = 4;
const uint8_t cols = 5;




DisplayTouchKeypad tkp(tft, ts, DisplayArea(0,50,320,240-50), rows, cols);

//Change these for your GFX sub class
const uint16_t black = ILI9341_BLACK;
const uint16_t white = ILI9341_WHITE;
const uint16_t red = ILI9341_RED;
const uint16_t green = ILI9341_GREEN;
const uint16_t blue = ILI9341_BLUE;

/**
 * A noddy class for this example
 */
class Foo {

  public:
    /**
     * A class method to handle the touch event
     * Can be called anything but requires ThouchKey& 
     * as its only parameter.
     * 
      */
    void onKeyTouched(TouchKey& key) {
      Serial.println("key touched");
    }
    
};

/**
 * Instantiate the class
 */
Foo foo;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(200);
  Serial.println("Class Method Lambda Callback DisplayTouchKeypad");
  tft.begin();
  tft.setRotation(1);
  tft.fillScreen(black);

  // For this example we set all keys to use the 
  // same touch handler
  for ( uint8_t row=0; row<rows; row++ ) {
    for ( uint8_t col=0; col<cols; col++ ) {
      //Get each key and set the handler so we can see events in Serial Monitor
      tkp.key(row, col).setTouchedHandler([&](TouchKey &key) { foo.onKeyTouched(key); });
    }
  }



}

void loop() {
  // Update the TouchKeypad and draw the keys (no labels
  // in this example)
  tkp.updateAndDraw();

}
