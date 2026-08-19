/*
 * ============================================================
 * LOOP SWITCHER v2.0 — Arduino Mega 2560
 * 5 Loops (bypass LOW / ativo HIGH) + Mute
 * PCF8574 LEDs | OLED RGB 1.5" | Encoder | 5 Botoes Preset
 * 
 * BOOT:    Loop 1 ativo, demais bypass. Logo ASCII 2.5s.
 * PLAY:    Botoes 1-5 = carrega presets. Encoder click = Mute.
 *          Encoder girar = entra CONFIG. 3x click = Reset.
 * CONFIG:  Navega menu com encoder. Long press 3s = executa.
 *          Salva/Carrega/Apaga presets. Nao ultrapassa limites.
 * ============================================================
 */

#include <SPI.h>
#include <Wire.h>
#include <EEPROM.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1351.h>

#include "config.h"
#include "logo.h"
#include "pcf_manager.h"
#include "encoder_handler.h"
#include "button_handler.h"
#include "preset_manager.h"
#include "display_manager.h"
#include "state_machine.h"

PCFManager pcf;
EncoderHandler encoder;
ButtonHandler buttons;
PresetManager presets;
DisplayManager display;
StateMachine st;

uint32_t bootTimer = 0;
uint32_t muteBlinkTimer = 0;
bool muteBlinkState = false;

// ============================================================
// SETUP
// ============================================================

void setup() {
  Serial.begin(115200);

  // Relés: LOW=bypass, HIGH=loop ativo
  for (uint8_t i = 0; i < NUM_LOOPS; i++) {
    pinMode(RELAY_PINS[i], OUTPUT);
    digitalWrite(RELAY_PINS[i], LOW);
  }
  pinMode(MUTE_RELAY_PIN, OUTPUT);
  digitalWrite(MUTE_RELAY_PIN, LOW);

  // LED RGB
  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);
  setRgb(1, 0, 0); // Vermelho = inicializando

  // Inicializa subsistemas
  pcf.begin();
  encoder.begin();
  buttons.begin();
  presets.begin();
  display.begin();

  // Estado inicial: Loop 1 ativo (padrao de fabrica)
  st.resetToFactory();
  updateHardware();

  // Tela de boot
  display.showBootLogo();
  bootTimer = millis();

  Serial.println(F("Loop Switcher v2.0 pronto"));
}

// ============================================================
// LOOP PRINCIPAL
// ============================================================

void loop() {
  encoder.update();
  buttons.update();
  updateRgb();

  switch (st.getState()) {
    case STATE_BOOT:   handleBoot();   break;
    case STATE_PLAY:   handlePlay();   break;
    case STATE_CONFIG: handleConfig(); break;
  }

  updateRelays();
  delay(1);
}

// ============================================================
// HANDLERS DE ESTADO
// ============================================================

void handleBoot() {
  if (millis() - bootTimer >= BOOT_SCREEN_MS) {
    st.setState(STATE_PLAY);
    display.drawPlay(st.getLoops(), st.getMute(), 255, st.getEditPreset());
    Serial.println(F(">> MODO PLAY"));
  }
}

void handlePlay() {
  // ----- Botoes 1-5: carregar presets -----
  uint8_t btn = buttons.getPressed();
  if (btn != 255) {
    loadPreset(btn);
    return;
  }

  // ----- Encoder girado: entra em CONFIG -----
  int8_t delta = encoder.getDelta();
  if (delta != 0) {
    st.setState(STATE_CONFIG);
    st.setMenuItem(CFG_PRESET_SELECT);
    // Carrega preset atual para edicao (se existir)
    Preset p = presets.load(st.getEditPreset());
    if (p.active) {
      st.setLoops(p.loops);
      st.setMute(p.mute == 1);
    }
    display.drawConfig(st.getMenuItem(), st.getEditPreset(), st.getLoops(), st.getMute());
    return;
  }

  // ----- Encoder click simples: toggle MUTE -----
  if (encoder.singleClick()) {
    st.toggleMute();
    updateHardware();
    display.drawPlay(st.getLoops(), st.getMute(), st.getPlayPreset(), st.getEditPreset());
    return;
  }

  // ----- 3 cliques curtos: Reset de fabrica -----
  if (encoder.tripleClick()) {
    st.resetToFactory();
    st.setPlayPreset(255);
    updateHardware();
    display.drawMessage("Reset de fabrica!", C_GREEN);
    delay(600);
    display.drawPlay(st.getLoops(), st.getMute(), 255, st.getEditPreset());
    return;
  }
}

void handleConfig() {
  int8_t delta = encoder.getDelta();

  if (delta != 0) {
    uint8_t item = st.getMenuItem();

    if (item == CFG_PRESET_SELECT) {
      st.changeEditPreset(delta);
      // Carrega preset selecionado para visualizacao
      Preset p = presets.load(st.getEditPreset());
      if (p.active) {
        st.setLoops(p.loops);
        st.setMute(p.mute == 1);
      }
    } else {
      st.changeMenuItem(delta);
    }

    display.drawConfig(st.getMenuItem(), st.getEditPreset(), st.getLoops(), st.getMute());
    return;
  }

  // ----- Long press 3s: executa acao do item -----
  if (encoder.longPress()) {
    executeConfigAction();
    return;
  }

  // ----- 3 cliques: volta ao PLAY sem salvar -----
  if (encoder.tripleClick()) {
    st.setState(STATE_PLAY);
    display.drawPlay(st.getLoops(), st.getMute(), st.getPlayPreset(), st.getEditPreset());
  }
}

void executeConfigAction() {
  uint8_t item = st.getMenuItem();

  switch (item) {
    case CFG_PRESET_SELECT:
      // Apenas visualiza, nada a executar
      break;

    case CFG_LOOP_1:
    case CFG_LOOP_2:
    case CFG_LOOP_3:
    case CFG_LOOP_4:
    case CFG_LOOP_5:
      st.toggleLoop(item - CFG_LOOP_1);
      updateHardware();
      break;

    case CFG_MUTE:
      st.toggleMute();
      updateHardware();
      break;

    case CFG_SAVE:
      presets.saveCurrent(st.getEditPreset(), st.getLoops(), st.getMute());
      st.setPlayPreset(st.getEditPreset());
      display.drawSaving(st.getEditPreset());
      setRgb(1, 1, 0); // Amarelo fixo = gravando
      delay(SAVE_FLASH_MS);
      break;

    case CFG_BACK:
      st.setState(STATE_PLAY);
      display.drawPlay(st.getLoops(), st.getMute(), st.getPlayPreset(), st.getEditPreset());
      return;
  }

  if (st.getState() == STATE_CONFIG) {
    display.drawConfig(st.getMenuItem(), st.getEditPreset(), st.getLoops(), st.getMute());
  }
}

// ============================================================
// FUNCOES DE HARDWARE
// ============================================================

void loadPreset(uint8_t idx) {
  Preset p = presets.load(idx);
  if (!p.active) {
    display.drawMessage("Preset vazio!", C_RED);
    delay(400);
    display.drawPlay(st.getLoops(), st.getMute(), st.getPlayPreset(), st.getEditPreset());
    return;
  }

  st.setLoops(p.loops);
  st.setMute(p.mute == 1);
  st.setPlayPreset(idx);
  updateHardware();

  display.drawPlay(st.getLoops(), st.getMute(), idx, st.getEditPreset());
  Serial.print(F("Preset ")); Serial.print(idx + 1); Serial.println(F(" carregado"));
}

void updateHardware() {
  updateRelays();
  updatePcfLeds();
}

void updateRelays() {
  uint8_t loops = st.getLoops();
  for (uint8_t i = 0; i < NUM_LOOPS; i++) {
    digitalWrite(RELAY_PINS[i], (loops >> i) & 1);
  }
  digitalWrite(MUTE_RELAY_PIN, st.getMute() ? HIGH : LOW);
}

void updatePcfLeds() {
  pcf.setFromState(st.getLoops(), st.getMute());
}

void setRgb(bool r, bool g, bool b) {
  digitalWrite(LED_R, r ? HIGH : LOW);
  digitalWrite(LED_G, g ? HIGH : LOW);
  digitalWrite(LED_B, b ? HIGH : LOW);
}

void updateRgb() {
  if (st.getState() == STATE_BOOT) {
    setRgb(1, 0, 0);
    return;
  }

  // Amarelo fixo durante salvamento
  if (st.getState() == STATE_CONFIG) {
    // Verifica se acabou de salvar (nao tem flag, mas podemos inferir)
    // Na pratica, o delay no save ja passou quando volta aqui
  }

  // Amarelo piscando = mute ativo
  if (st.getMute()) {
    if (millis() - muteBlinkTimer >= MUTE_BLINK_MS) {
      muteBlinkTimer = millis();
      muteBlinkState = !muteBlinkState;
      setRgb(muteBlinkState ? 1 : 0, muteBlinkState ? 1 : 0, 0);
    }
    return;
  }

  // Verde = sistema pronto
  setRgb(0, 1, 0);
}
