#ifndef PCF_MANAGER_H
#define PCF_MANAGER_H

#include <Wire.h>
#include "config.h"

class PCFManager {
private:
  uint8_t _state = 0xFF; // HIGH = LED apagado (PCF8574 com cátodo comum)

public:
  void begin() {
    Wire.begin();
    write(0xFF);
  }

  void write(uint8_t data) {
    _state = data;
    Wire.beginTransmission(PCF_ADDR);
    Wire.write(_state);
    Wire.endTransmission();
  }

  // LED do loop (0-4) ou mute (5). true = aceso
  void setLed(uint8_t ch, bool on) {
    if (ch > 5) return;
    if (on) _state &= ~(1 << ch);   // LOW = aceso
    else    _state |=  (1 << ch);   // HIGH = apagado
    write(_state);
  }

  void setFromState(uint8_t loopBits, bool mute) {
    for (uint8_t i = 0; i < 5; i++) {
      setLed(i, loopBits & (1 << i));
    }
    setLed(5, mute);
  }

  void clearAll() {
    write(0xFF);
  }
};

#endif
