#ifndef ENCODER_HANDLER_H
#define ENCODER_HANDLER_H

#include <Arduino.h>
#include "config.h"

class EncoderHandler {
private:
  volatile int8_t _delta = 0;
  uint8_t _lastClk = HIGH;

  uint8_t _swState = HIGH;
  uint8_t _lastSwState = HIGH;
  uint32_t _swDownTime = 0;
  uint32_t _swUpTime = 0;
  uint8_t _clickCount = 0;

  bool _longPressFlag = false;
  bool _tripleClickFlag = false;
  bool _singleClickFlag = false;

  static EncoderHandler* _instance;

  static void isr() {
    if (!_instance) return;
    uint8_t clk = digitalRead(ENCODER_CLK);
    uint8_t dt  = digitalRead(ENCODER_DT);
    if (clk != _instance->_lastClk && clk == LOW) {
      _instance->_delta += (dt == HIGH) ? 1 : -1;
    }
    _instance->_lastClk = clk;
  }

public:
  void begin() {
    _instance = this;
    pinMode(ENCODER_CLK, INPUT_PULLUP);
    pinMode(ENCODER_DT,  INPUT_PULLUP);
    pinMode(ENCODER_SW,  INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(ENCODER_CLK), isr, CHANGE);
  }

  void update() {
    uint8_t r = digitalRead(ENCODER_SW);

    if (r != _lastSwState) {
      delayMicroseconds(500);
      r = digitalRead(ENCODER_SW);
    }

    if (r != _swState) {
      _swState = r;
      if (_swState == LOW) {
        _swDownTime = millis();
        _longPressFlag = false;
      } else {
        _swUpTime = millis();
        if (_swUpTime - _swDownTime < LONG_PRESS_MS) {
          _clickCount++;
        }
      }
    }

    if (_swState == LOW && !_longPressFlag) {
      if (millis() - _swDownTime >= LONG_PRESS_MS) {
        _longPressFlag = true;
        _clickCount = 0;
      }
    }

    if (_clickCount > 0 && millis() - _swUpTime > TRIPLE_CLICK_WINDOW) {
      if (_clickCount == 1) _singleClickFlag = true;
      else if (_clickCount >= 3) _tripleClickFlag = true;
      _clickCount = 0;
    }
  }

  int8_t getDelta() {
    int8_t d = _delta;
    _delta = 0;
    return d;
  }

  bool longPress() {
    if (_longPressFlag) { _longPressFlag = false; return true; }
    return false;
  }

  bool singleClick() {
    if (_singleClickFlag) { _singleClickFlag = false; return true; }
    return false;
  }

  bool tripleClick() {
    if (_tripleClickFlag) { _tripleClickFlag = false; return true; }
    return false;
  }

  void reset() {
    _delta = 0; _clickCount = 0;
    _longPressFlag = false;
    _singleClickFlag = false;
    _tripleClickFlag = false;
  }
};

EncoderHandler* EncoderHandler::_instance = nullptr;

#endif
