#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1351.h>
#include "config.h"
#include "logo.h"

#define C_BLACK  0x0000
#define C_WHITE  0xFFFF
#define C_RED    0xF800
#define C_GREEN  0x07E0
#define C_BLUE   0x001F
#define C_YELLOW 0xFFE0
#define C_ORANGE 0xFD20
#define C_GRAY   0x8410
#define C_CYAN   0x07FF
#define C_DKGREEN 0x03E0

class DisplayManager {
private:
  Adafruit_SSD1351 _tft;

public:
  DisplayManager() : _tft(128, 128, &SPI, OLED_CS, OLED_DC, OLED_RST) {}

  void begin() {
    _tft.begin();
    _tft.setRotation(0);
    _tft.fillScreen(C_BLACK);
  }

  void showBootLogo() {
    _tft.fillScreen(C_BLACK);
    _tft.setTextSize(1);
    _tft.setTextColor(C_CYAN);
    int16_t y0 = (128 - LOGO_LINES * 6) / 2;
    int16_t x0 = (128 - 21 * 6) / 2;
    for (uint8_t i = 0; i < LOGO_LINES; i++) {
      _tft.setCursor(x0, y0 + i * 6);
      _tft.print(BOOT_LOGO[i]);
    }
  }

  // ========== MODO PLAY (ao vivo) ==========
  void drawPlay(uint8_t loops, bool mute, uint8_t presetLoaded, uint8_t presetEditing) {
    _tft.fillScreen(C_BLACK);

    // Cabeçalho
    _tft.setTextColor(C_WHITE);
    _tft.setCursor(25, 4);
    _tft.print(F("MODO PLAY"));

    // Linha divisória
    _tft.drawFastHLine(0, 14, 128, C_GRAY);

    // Preset ativo
    _tft.setTextColor(C_YELLOW);
    _tft.setCursor(6, 20);
    if (presetLoaded < 5) {
      _tft.print(F("Preset: ")); _tft.print(presetLoaded + 1);
    } else {
      _tft.print(F("Preset: --"));
    }

    // Indicadores dos 5 loops
    const char* names[5] = {"L1", "L2", "L3", "L4", "L5"};
    for (uint8_t i = 0; i < 5; i++) {
      uint8_t x = 6 + i * 24;
      bool on = loops & (1 << i);
      _tft.fillRoundRect(x, 36, 22, 22, 3, on ? C_GREEN : C_GRAY);
      _tft.setTextColor(on ? C_BLACK : C_WHITE);
      _tft.setCursor(x + 4, 42);
      _tft.print(names[i]);
    }

    // Mute
    _tft.fillRoundRect(46, 64, 36, 16, 3, mute ? C_RED : C_GRAY);
    _tft.setTextColor(C_WHITE);
    _tft.setCursor(52, 68);
    _tft.print(F("MUTE"));

    // Instruções
    _tft.setTextColor(C_CYAN);
    _tft.setCursor(6, 88);
    _tft.print(F("Bt 1-5: Carrega"));
    _tft.setCursor(6, 98);
    _tft.print(F("Enc: Config"));
    _tft.setCursor(6, 108);
    _tft.print(F("Click: Mute"));
    _tft.setCursor(6, 118);
    _tft.print(F("3xClick: Reset"));

    // Preset sendo editado (se em config)
    if (presetEditing < 5) {
      _tft.setTextColor(C_ORANGE);
      _tft.setCursor(80, 20);
      _tft.print(F("Ed:P")); _tft.print(presetEditing + 1);
    }
  }

  // ========== MODO CONFIG ==========
  void drawConfig(uint8_t item, uint8_t presetEditing, uint8_t loops, bool mute) {
    _tft.fillScreen(C_BLACK);

    _tft.setTextColor(C_WHITE);
    _tft.setCursor(22, 4);
    _tft.print(F("CONFIGURAR"));

    _tft.drawFastHLine(0, 14, 128, C_GRAY);

    const char* labels[] = {
      "Preset",
      "Loop 1",
      "Loop 2",
      "Loop 3",
      "Loop 4",
      "Loop 5",
      "Mute",
      "SALVAR",
      "VOLTAR"
    };

    // Scroll: mostra 8 itens, ajusta janela
    uint8_t first = (item > 7) ? item - 7 : 0;
    for (uint8_t i = first; i < CFG_COUNT && (i - first) < 8; i++) {
      uint8_t y = 18 + (i - first) * 13;
      bool sel = (i == item);

      if (sel) {
        _tft.fillRect(0, y - 1, 128, 12, C_BLUE);
        _tft.setTextColor(C_BLACK);
      } else {
        _tft.setTextColor(C_WHITE);
      }

      _tft.setCursor(4, y);
      _tft.print(labels[i]);

      // Valores
      if (i == CFG_PRESET_SELECT) {
        _tft.setCursor(90, y);
        _tft.print(F("P")); _tft.print(presetEditing + 1);
      } else if (i >= CFG_LOOP_1 && i <= CFG_LOOP_5) {
        uint8_t li = i - CFG_LOOP_1;
        bool on = loops & (1 << li);
        _tft.setCursor(90, y);
        _tft.print(on ? F("[ON]") : F("[OFF]"));
      } else if (i == CFG_MUTE) {
        _tft.setCursor(90, y);
        _tft.print(mute ? F("[ON]") : F("[OFF]"));
      }
    }
  }

  void drawMessage(const char* txt, uint16_t color, uint16_t bg = C_BLACK) {
    _tft.fillScreen(bg);
    _tft.setTextColor(color);
    _tft.setTextSize(1);
    uint8_t len = strlen(txt) * 6;
    _tft.setCursor((128 - len) / 2, 60);
    _tft.print(txt);
  }

  void drawSaving(uint8_t preset) {
    _tft.fillScreen(C_BLACK);
    _tft.setTextColor(C_YELLOW);
    _tft.setCursor(10, 50);
    _tft.print(F("SALVANDO PRESET "));
    _tft.print(preset + 1);
    _tft.setCursor(30, 65);
    _tft.print(F("Aguarde..."));
  }
};

#endif
