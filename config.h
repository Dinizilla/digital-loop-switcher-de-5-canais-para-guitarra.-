#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// ============================================================
// PINOS — ARDUINO MEGA 2560
// ============================================================

// Relés dos 5 loops (HIGH = sinal entra no pedal / LOW = bypass)
const uint8_t RELAY_PINS[5] = {22, 24, 26, 28, 30};

// Relé do canal Mute (afinador)
const uint8_t MUTE_RELAY_PIN = 32;

// 5 botões de preset ao vivo (pull-up interno)
const uint8_t PRESET_BUTTONS[5] = {34, 36, 38, 40, 42};

// Encoder rotativo
const uint8_t ENCODER_CLK = 2;   // Interrupção
const uint8_t ENCODER_DT  = 3;   // Interrupção
const uint8_t ENCODER_SW  = 4;   // Botão com pull-up

// LED RGB de status (cátodo comum)
const uint8_t LED_R = 5;
const uint8_t LED_G = 6;
const uint8_t LED_B = 7;

// PCF8574 (endereço I2C — A0=A1=A2=GND → 0x20)
const uint8_t PCF_ADDR = 0x20;

// Display OLED RGB SSD1351 (SPI)
const uint8_t OLED_CS  = 10;
const uint8_t OLED_DC  = 9;
const uint8_t OLED_RST = 8;

// ============================================================
// CONSTANTES
// ============================================================

const uint8_t NUM_LOOPS = 5;
const uint8_t NUM_PRESETS = 10;      // 5 botões + 5 extras via menu
const uint16_t EEPROM_BASE = 0;

// Tempos (ms)
const uint16_t DEBOUNCE_MS = 30;
const uint16_t LONG_PRESS_MS = 3000;
const uint16_t TRIPLE_CLICK_WINDOW = 500;
const uint16_t BOOT_SCREEN_MS = 2500;
const uint16_t MUTE_BLINK_MS = 250;
const uint16_t SAVE_FLASH_MS = 600;

// Estados do sistema
enum SystemState {
  STATE_BOOT,
  STATE_PLAY,      // Modo ao vivo — botões carregam presets
  STATE_CONFIG     // Modo configuração — encoder edita e salva
};

// Itens do menu de configuração
enum ConfigItem {
  CFG_PRESET_SELECT,
  CFG_LOOP_1,
  CFG_LOOP_2,
  CFG_LOOP_3,
  CFG_LOOP_4,
  CFG_LOOP_5,
  CFG_MUTE,
  CFG_SAVE,
  CFG_BACK,
  CFG_COUNT
};

#endif
