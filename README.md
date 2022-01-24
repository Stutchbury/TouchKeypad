# TouchKeypad

A library for Arduino/Teensy etc that provides (relatively) easy creation of a keypad touch interface with events fired (touch and repeat touch) with visual feedback of touch interactions.

Each key can be assigned its own touch callback handler (or use a common one) and the display class can also have a draw function attached for icons etc..


![Offset Keypad](images/offset_keypad.jpeg)

*NOTE:* This is very much beta software. The API is likely to change and it currently expects a display of 320x240 with GFX ```setRotation(1)```.

## TouchKey Class

Created automagically by the ```TouchKeypad``` class, this would not normally be instatiated outside of that context.

The area of each key will be able to be changed (but not in this release).

Extends the [```DisplayUtils```](https://github.com/Stutchbury/DisplayUtils) Area



## TouchKeypad Class

Creates a keypad of ```TouchKey```s

Extends the [```DisplayUtils```](https://github.com/Stutchbury/DisplayUtils) Area


Requires Adafruit's [```TouchScreen.h```](https://github.com/adafruit/Adafruit_TouchScreen)





## DisplayTouchKeypad Class

A subclass of TouchKeypad.

Requires Adafruit's [GFX library](https://github.com/adafruit/Adafruit-GFX-Library) (or rather, a subclass of it for your display).

This is the class that actually draws stuff on the display - either instantiate or subclass this one.

The ```update(uint16_t touchUpdateMs=10)``` and ```draw(uint16_t displayRefreshMs=100)``` methods are provide separately as it is often necessary to control the timing of drawing.
Both accept a milliseconds parameter to limit update/draw frequency.


An ```updateAndDraw()``` method is provided for convenience.


