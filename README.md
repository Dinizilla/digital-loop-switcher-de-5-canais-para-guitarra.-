🔁 Loop Switcher v2.0
⚠️ ESTADO ATUAL: WORK IN PROGRESS (WIP)
Este projeto ainda não está funcionando 100%. É baseado no projeto que finalizei há uns 2 anos, mas estou refazendo para ficar mais compacto, com menos peças.  Algumas partes podem precisar de ajustes de timing, lógica invertida de relés, endereçamento I2C, ou calibração do encoder. Use por sua conta e risco e contribua com melhorias!
📖 Visão Geral
Loop Switcher para pedais de efeitos analógicos de guitarra, baseado no Arduino Mega 2560.
5 canais Send/Return com relés (bypass/ativo)
1 canal Mute dedicado ao pedal afinador
5 botões físicos para carregamento instantâneo de presets ao vivo
Encoder rotativo com click para navegação e configuração
Display OLED RGB 1.5" (SSD1351) para interface visual
PCF8574 para controle dos LEDs indicadores
LED RGB de status do sistema
EEPROM interna para persistência de presets
🏗️ Estrutura de Arquivos
plain
/LoopSwitcher
├── LoopSwitcher.ino      # Arquivo principal (setup + loop + máquina de estados)
├── config.h              # Pinos, constantes, enums e tempos
├── logo.h                # Logo ASCII editável na tela de boot
├── pcf_manager.h         # Driver I2C para o PCF8574 (LEDs)
├── encoder_handler.h     # Encoder com debounce, interrupções e gestos
├── button_handler.h      # 5 botões de preset com debounce
├── preset_manager.h      # Gerenciamento de presets na EEPROM
├── display_manager.h     # Renderização do OLED SSD1351
└── state_machine.h       # Estados e variáveis do sistema
🔌 Hardware Necessário
Planilhas
Componente	Especificação	Quantidade
Microcontrolador	Arduino Mega 2560 Pro	1
Display	OLED RGB 1.5" SSD1351 (SPI)          	1
Expansor I/O	PCF8574 (I2C)          	1 (o antigo usava 2)
Relés	Módulo relé 5V com optoacoplador	6 (5 loops + 1 mute)
Encoder	Rotativo com botão integrado	          1
Botões	Push-button momentâneo	          6 a 8
LED RGB	Cátodo comum	                    1
LEDS                                              6 (da cor que desejar)
Resistores          220Ω                     	6 para os leds
Fonte	9V DC (pedais) + 5V DC (Arduino)	—
📍 Pinagem (Arduino Mega 2560)
Planilhas
Pino Arduino	Função	Tipo
22	Relé — Loop 1	Saída
24	Relé — Loop 2	Saída
26	Relé — Loop 3	Saída
28	Relé — Loop 4	Saída
30	Relé — Loop 5	Saída
32	Relé — Mute (Afinador)	Saída
34	Botão — Preset 1	Entrada (pull-up)
36	Botão — Preset 2	Entrada (pull-up)
38	Botão — Preset 3	Entrada (pull-up)
40	Botão — Preset 4	Entrada (pull-up)
42	Botão — Preset 5	Entrada (pull-up)
2	Encoder — CLK	Entrada (interrupção)
3	Encoder — DT	Entrada (interrupção)
4	Encoder — SW (botão)	Entrada (pull-up)
5	LED RGB — Vermelho	Saída
6	LED RGB — Verde	Saída
7	LED RGB — Azul	Saída
8	OLED — RST	Saída
9	OLED — DC	Saída
10	OLED — CS	Saída
20 (SDA)	PCF8574 — SDA	I2C
21 (SCL)	PCF8574 — SCL	I2C
50-53	OLED — SPI (MISO, MOSI, SCK, SS)	SPI Hardware
Nota: Os pinos SPI do Mega 2560 são fixos: 50 (MISO), 51 (MOSI), 52 (SCK). O pino 10 é usado como CS do display.
🛠️ Como Compilar e Instalar
1. Instalar as bibliotecas
Abra o Arduino IDE ou PlatformIO e instale:
Adafruit GFX Library
Adafruit SSD1351
2. Verificar o endereço I2C do PCF8574
Com todos os jumpers A0, A1, A2 aterrados (GND), o endereço é 0x20. Se você usar jumpers diferentes, ajuste em config.h:
cpp
const uint8_t PCF_ADDR = 0x20;  // Ajuste conforme necessário
3. Verificar a lógica dos relés
O código assume:
LOW  → Bypass (sinal passa direto)
HIGH → Ativo (sinal entra no pedal do loop)
Se o seu módulo de relés tiver lógica invertida (alguns módulos ativam com LOW), inverta os valores em updateRelays() dentro de LoopSwitcher.ino.
4. Carregar o código
Coloque todos os arquivos .h e .ino na mesma pasta /LoopSwitcher/
Abra LoopSwitcher.ino no Arduino IDE
Selecione a placa Arduino Mega 2560
Compile e faça o upload
🎮 Modos de Operação
🎸 MODO PLAY (Ao Vivo)
Este é o modo padrão após a inicialização.
Planilhas
Ação	Resultado
Botão 1-5	Carrega o preset correspondente (loops + mute salvos)
Click no encoder	Liga/desliga o Mute (afinador)
Girar o encoder	Entra no Modo Config
3x click rápido no encoder	Reset de fábrica: Loop 1 ativo, demais bypass, mute off
Estado inicial: Ao ligar, o sistema mostra a logo por 2.5s e entra no modo PLAY com apenas o Loop 1 ativo (os demais em bypass).
⚙️ MODO CONFIG (Configuração)
Acesse girando o encoder no modo PLAY.
Planilhas
Ação	Resultado
Girar encoder	Navega entre os itens do menu (limitado aos extremos, não dá a volta)
Girar em "Preset"	Seleciona qual preset (1-5) está sendo editado
Long press 3s	Executa a ação do item selecionado
3x click rápido	Volta ao modo PLAY sem salvar
Itens do Menu:
Planilhas
Item	Descrição
Preset	Seleciona P1-P5 para edição. Carrega o preset se existir.
Loop 1-5	Toggle ON/OFF do loop (long press para alternar)
Mute	Toggle ON/OFF do mute (long press para alternar)
SALVAR	Salva a configuração atual no preset selecionado (EEPROM)
VOLTAR	Retorna ao modo PLAY
Dica: O preset sendo editado é mostrado no canto superior direito da tela no modo PLAY.
🎨 Personalizando a Logo
Edite o arquivo logo.h livremente. Regras:
Mantenha 21 linhas (ou ajuste LOGO_LINES)
Mantenha 21 colunas de largura para centralizar no display 128x128
Use apenas caracteres ASCII (#, @, %, *, +, -, =, ., espaço)
A logo é exibida por 2.5 segundos na inicialização
cpp
const char* BOOT_LOGO[] = {
  "    #############    ",
  "  ##             ##  ",
  // ... edite aqui
};
🚦 LED RGB — Significado das Cores
Planilhas
Cor	Estado	Significado
🔴 Vermelho	Contínuo	Sistema inicializando (boot)
🟢 Verde	Contínuo	Sistema pronto — modo PLAY
🟡 Amarelo piscando	Piscante	Mute ativo (afinador ligado)
🟡 Amarelo fixo	Contínuo	Salvando preset na EEPROM
🗺️ Mapeamento dos LEDs do PCF8574
Planilhas
Bit	LED	Descrição
P0	Loop 1	Aceso = Loop ativo
P1	Loop 2	Aceso = Loop ativo
P2	Loop 3	Aceso = Loop ativo
P3	Loop 4	Aceso = Loop ativo
P4	Loop 5	Aceso = Loop ativo
P5	Mute	Aceso = Mute ativo
P6	Livre	Reservado para expansão
P7	Livre	Reservado para expansão
O PCF8574 trabalha com lógica invertida para LEDs: LOW no pino = LED aceso.

⚠️ Problemas Conhecidos & Limitações
Este projeto ainda não está funcionando 100%. Abaixo estão os pontos que podem precisar de ajustes:
Não testado em hardware real
O código foi escrito e revisado logicamente, mas não foi validado fisicamente. Comportamentos de timing, bouncing de chaves e resposta do display podem variar.
Lógica dos relés
Dependendo do módulo de relé usado, a ativação pode ser HIGH ou LOW. Verifique o datasheet do seu módulo e ajuste updateRelays() se necessário.
Endereço I2C do PCF8574
O endereço padrão é 0x20. Se o seu chip tiver jumpers diferentes ou for um PCF8574A, o endereço pode ser outro (ex: 0x38). Use um scanner I2C para confirmar.
Encoder — detecção de gestos
A janela de tempo para distinguish single click / triple click pode precisar de calibração. Ajuste TRIPLE_CLICK_WINDOW em config.h se o encoder não responder como esperado.
Display OLED
Certifique-se de que seu display usa o driver SSD1351. Displays com outros controladores (SSD1306, SH1106, etc.) não funcionarão sem adaptação.
Consumo de corrente
6 relés + display OLED + LEDs podem consumir mais corrente do que o Arduino fornece via USB. Use uma fonte externa de 5V/1A ou mais para o Arduino.
EEPROM
A EEPROM do Arduino Mega tem ~100.000 ciclos de escrita. Evite salvar presets em loop infinito. O código já protege contra gravação acidental, mas cuidado em testes.
Fonte do display
As fontes do Adafruit GFX são pequenas. Se precisar de fontes maiores ou customizadas, será necessário incluir arquivos de fonte adicionais.

🔮 Funcionalidades Futuras (Ideias)

[ ] Bancos de presets (ex: 5 bancos × 5 presets = 25 combinações)
[ ] Backup/restauração de presets via Serial
[ ] Fade in/out, ou algo do tipo, ao trocar de preset para evitar estalos
