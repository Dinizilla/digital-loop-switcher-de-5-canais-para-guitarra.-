#ifndef BUTTON_HANDLER_H
#define BUTTON_HANDLER_H

#include <Arduino.h>
#include "config.h"

class ButtonHandler {
private:
  uint8_t _state[5] = {HIGH, HIGH, HIGH, HIGH, HIGH};
  uint8_t _lastState[5] = {HIGH, HIGH, HIGH, HIGH, HIGH};
  uint32_t _debounceTime[5] = {0, 0, 0, 0, 0};
  bool _triggered[5] = {false, false, false, false, false};

public:
  void begin() {
    for (uint8_t i = 0; i < 5; i++) {
      pinMode(PRESET_BUTTONS[i], INPUT_PULLUP);
    }
  }

  void update() {
    for (uint8_t i = 0; i < 5; i++) {
      uint8_t r = digitalRead(PRESET_BUTTONS[i]);

      if (r != _lastState[i]) {
        _debounceTime[i] = millis();
      }

      if ((millis() - _debounceTime[i]) > DEBOUNCE_MS) {
        if (r != _state[i]) {
          _state[i] = r;
          if (_state[i] == LOW) { // Pressionado
            _triggered[i] = true;
          }
        }
      }
      _lastState[i] = r;
    }
  }

  // Retorna 0-4 se algum botão foi pressionado, ou 255 se nenhum
  uint8_t getPressed() {
    for (uint8_t i = 0; i < 5; i++) {
      if (_triggered[i]) {
        _triggered[i] = false;
        return i;
      }
    }
    return 255;
  }
};

#endif
