#ifndef PRESET_MANAGER_H
#define PRESET_MANAGER_H

#include <EEPROM.h>
#include "config.h"

struct Preset {
  uint8_t loops;      // Bits 0-4 = loops 1-5
  uint8_t mute;       // 0 ou 1
  uint8_t active;     // 1 = válido
  uint8_t checksum;
};

class PresetManager {
private:
  uint16_t _addr(uint8_t idx) {
    return EEPROM_BASE + idx * sizeof(Preset);
  }

  uint8_t _checksum(const Preset& p) {
    return p.loops ^ p.mute ^ p.active ^ 0x55;
  }

public:
  void begin() {
    for (uint8_t i = 0; i < NUM_PRESETS; i++) {
      Preset p = load(i);
      if (p.active && p.checksum != _checksum(p)) {
        p.active = 0;
        save(i, p);
      }
    }
  }

  Preset load(uint8_t idx) {
    Preset p = {0, 0, 0, 0};
    if (idx >= NUM_PRESETS) return p;
    EEPROM.get(_addr(idx), p);
    return p;
  }

  void save(uint8_t idx, const Preset& p) {
    if (idx >= NUM_PRESETS) return;
    Preset tmp = p;
    tmp.checksum = _checksum(tmp);
    EEPROM.put(_addr(idx), tmp);
  }

  void saveCurrent(uint8_t idx, uint8_t loops, bool mute) {
    Preset p;
    p.loops = loops;
    p.mute = mute ? 1 : 0;
    p.active = 1;
    save(idx, p);
  }

  void deletePreset(uint8_t idx) {
    Preset p = {0, 0, 0, 0};
    save(idx, p);
  }

  bool isValid(uint8_t idx) {
    Preset p = load(idx);
    return p.active == 1 && p.checksum == _checksum(p);
  }
};

#endif
