#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include "config.h"

class StateMachine {
private:
  SystemState _state = STATE_BOOT;

  uint8_t _loops = 0;
  bool _mute = false;

  uint8_t _playPreset = 255;    // Preset carregado no modo PLAY (0-4 ou 255)
  uint8_t _editPreset = 0;      // Preset sendo editado no modo CONFIG (0-4)
  uint8_t _menuItem = 0;        // Item selecionado no menu CONFIG

public:
  SystemState getState() const { return _state; }
  void setState(SystemState s) { _state = s; }

  uint8_t getLoops() const { return _loops; }
  void setLoops(uint8_t v) { _loops = v; }
  void toggleLoop(uint8_t i) { if (i < 5) _loops ^= (1 << i); }

  bool getMute() const { return _mute; }
  void setMute(bool v) { _mute = v; }
  void toggleMute() { _mute = !_mute; }

  uint8_t getPlayPreset() const { return _playPreset; }
  void setPlayPreset(uint8_t p) { _playPreset = p; }

  uint8_t getEditPreset() const { return _editPreset; }
  void setEditPreset(uint8_t p) { if (p < 5) _editPreset = p; }
  void changeEditPreset(int8_t d) {
    int16_t v = (int16_t)_editPreset + d;
    if (v < 0) v = 4;
    if (v > 4) v = 0;
    _editPreset = (uint8_t)v;
  }

  uint8_t getMenuItem() const { return _menuItem; }
  void setMenuItem(uint8_t m) { if (m < CFG_COUNT) _menuItem = m; }
  void changeMenuItem(int8_t d) {
    int16_t v = (int16_t)_menuItem + d;
    if (v < 0) v = 0;
    if (v >= CFG_COUNT) v = CFG_COUNT - 1;
    _menuItem = (uint8_t)v;
  }

  void resetToFactory() {
    _loops = 0x01;   // Apenas Loop 1 ativo
    _mute = false;
    _playPreset = 255;
    _editPreset = 0;
    _menuItem = 0;
  }
};

#endif
