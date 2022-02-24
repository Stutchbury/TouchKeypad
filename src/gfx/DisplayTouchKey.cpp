/**
 *
 * GPLv2 Licence https://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 *
 * Copyright (c) 2022 Philip Fletcher <philip.fletcher@stutchbury.com>
 *
 */

#include "DisplayTouchKeypad.h"

DisplayTouchKeypad::DisplayTouchKeypad(Adafruit_GFX& d, TouchScreen& touchscreen, DisplayArea keypadArea, uint8_t rows, uint8_t cols)
  : TouchKeypad(touchscreen, keypadArea, rows, cols), gfx(d) {
}

DisplayTouchKeypad::DisplayTouchKeypad(Adafruit_GFX& d, const GFXfont* f, TouchScreen& touchscreen, DisplayArea keypadArea, uint8_t rows, uint8_t cols)
  : TouchKeypad(touchscreen, keypadArea, rows, cols), gfx(d), font(f) {
}

void DisplayTouchKeypad::updateAndDraw(uint16_t touchUpdateMs /*= 10*/, uint16_t displayRefreshMs /*= 100*/) {
  TouchKeypad::update(touchUpdateMs);
  draw(displayRefreshMs);
}

void DisplayTouchKeypad::draw(uint16_t displayRefreshMs /*= 100*/) {
  if (_enabled) {
    //Do event updates and fire touch handlers
    now = millis();
    //redraw as required
    if (now > lastDisplayRefresh + displayRefreshMs) {
      lastDisplayRefresh = now;
      if (forceRefresh) {
        gfx.fillRect(x(), y(), w(), h(), bgColour);
      }
      int16_t  x, y;
      uint16_t w, h;
      for (uint8_t row = 0; row < rows; row++) {
        for (uint8_t col = 0; col < cols; col++) {
          if (key(row, col).enabled()) {
            bool keyStale = (drawnUserId[row][col] != key(row, col).userId() || drawnUserState[row][col] != key(row, col).userState());
            drawnUserId[row][col] = key(row, col).userId();
            drawnUserState[row][col] = key(row, col).userState();
            //Draw the feedback flash
            if (key(row, col).lastTouched() > key(row, col).lastUntouched()) {
              //if ( key(row, col).enabled() ) {
              gfx.fillRoundRect(key(row, col).x() + keyMargin, key(row, col).y() + keyMargin, key(row, col).w() - (keyMargin * 2), key(row, col).h() - (keyMargin * 2), keyRadius, touchedColour);
              //}
              keyUntouched[row][col] = false;
            }
            else if (forceRefresh || keyStale || (!keyUntouched[row][col] && now > untouchDelayMs + key(row, col).lastTouched())) {
              gfx.fillRect(key(row, col).x(), key(row, col).y(), key(row, col).w(), key(row, col).h(), bgColour);
              if (keyLabelFunctions[row][col] != NULL) {
                keyLabelFunctions[row][col](keys[row][col]);
              }
              else {
                gfx.drawRoundRect(key(row, col).x() + keyMargin, key(row, col).y() + keyMargin, key(row, col).w() - (keyMargin * 2), key(row, col).h() - (keyMargin * 2), keyRadius, fgColour);
                if (strlen(labels[row][col]) > 0) {
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


void DisplayTouchKeypad::setLabel(uint8_t row, uint8_t col, char label[5]) {
  strcpy(labels[row][col], label);
}

void DisplayTouchKeypad::setBackgroundColour(uint16_t colour) {
  bgColour = colour;
}
void DisplayTouchKeypad::setForegroundColour(uint16_t colour) {
  fgColour = colour;
}
void DisplayTouchKeypad::setTouchedColour(uint16_t colour) {
  touchedColour = colour;
}



char* DisplayTouchKeypad::getLabel(uint8_t row, uint8_t col) {
  return labels[row][col];
}

void DisplayTouchKeypad::setDrawHandler(uint8_t row, uint8_t col, TouchKeyDrawFunction f) {
  keyLabelFunctions[row][col] = f;
}

void DisplayTouchKeypad::setFont(const GFXfont* f) {
  if (font == NULL || font != f) {
    font = f;
    //gfx.setFont(font);
  }
}

void DisplayTouchKeypad::clear() {
  gfx.fillRect(x(), y(), w(), h(), bgColour);
  forceRefresh = true;
}

