# 🔁 Loop Switcher v2.0

> ⚠️ **Status: Work in Progress (WIP)**
> Esta versão ainda não foi validada completamente em hardware real.

Loop Switcher programável para pedais de efeitos analógicos de guitarra, desenvolvido com **Arduino Mega 2560**.

O projeto é uma evolução de uma versão anterior que foi concluída há aproximadamente dois anos. Nesta nova versão, o objetivo é tornar o sistema **mais compacto, organizado e simples**, reduzindo a quantidade de componentes e centralizando o controle em um único **PCF8574**.

> **⚠️ Importante:** esta versão ainda pode apresentar problemas relacionados a timing, lógica dos relés, endereçamento I2C, debounce e calibração do encoder. Use por sua conta e risco e contribua com melhorias!

---

## 🎸 Visão geral

O Loop Switcher permite controlar vários pedais de efeitos analógicos através de loops **Send/Return** com relés.

### Principais recursos

* 🎛️ **5 loops Send/Return** independentes
* 🔇 **1 canal Mute** dedicado ao pedal afinador
* 🎚️ **5 presets** de acesso rápido através de botões físicos
* 🔄 **Encoder rotativo com botão** para navegação e configuração
* 🖥️ **Display OLED RGB 1.5" SSD1351** para interface visual
* 💡 **LEDs indicadores individuais** para cada loop
* 🌈 **LED RGB** para indicação do estado do sistema
* 💾 **EEPROM interna** para armazenamento persistente dos presets
* 🔌 **PCF8574 via I2C** para expansão das saídas dos LEDs
* ⚙️ Máquina de estados para gerenciamento dos modos de operação

### Arquitetura simplificada

```text
                    ┌──────────────────────┐
                    │    Arduino Mega      │
                    │        2560          │
                    └──────────┬───────────┘
                               │
              ┌────────────────┼────────────────┐
              │                │                │
              ▼                ▼                ▼
        ┌──────────┐     ┌──────────┐     ┌──────────┐
        │  OLED    │     │ Encoder  │     │  Botões  │
        │ SSD1351  │     │          │     │ Presets  │
        └──────────┘     └──────────┘     └──────────┘
              │
              │ SPI
              │
              ▼
        ┌──────────┐
        │ PCF8574  │
        │   I2C    │
        └────┬─────┘
             │
             ▼
       LEDs dos loops

        Arduino Mega
             │
             ▼
       6 módulos de relé
             │
       ┌─────┴─────┐
       │           │
    Loops 1–5    Mute
```

---

## 📁 Estrutura do projeto

```text
/LoopSwitcher
│
├── LoopSwitcher.ino       # Arquivo principal
│                           # setup(), loop() e controle geral
│
├── config.h                # Pinos, constantes, enums e tempos
│
├── logo.h                  # Logo exibida durante o boot
│
├── pcf_manager.h           # Controle do PCF8574 via I2C
│
├── encoder_handler.h       # Encoder, debounce, interrupções e gestos
│
├── button_handler.h        # Botões dos presets e debounce
│
├── preset_manager.h        # Leitura e gravação de presets na EEPROM
│
├── display_manager.h       # Renderização do OLED SSD1351
│
└── state_machine.h         # Estados e variáveis do sistema
```

A estrutura foi separada por responsabilidades para facilitar manutenção, testes e futuras expansões.

---

# 🔌 Hardware

## Componentes necessários

| Componente       | Especificação                             | Quantidade |
| ---------------- | ----------------------------------------- | ---------: |
| Microcontrolador | Arduino Mega 2560 Pro                     |          1 |
| Display          | OLED RGB 1.5" SSD1351 — SPI               |          1 |
| Expansor I/O     | PCF8574 — I2C                             |          1 |
| Módulo de relés  | Relé 5V com optoacoplador                 |          6 |
| Encoder          | Rotativo com botão integrado              |          1 |
| Botões           | Push-button momentâneo                    |          5 |
| LED RGB          | Cátodo comum                              |          1 |
| LEDs indicadores | LED simples                               |          6 |
| Resistores       | 220 Ω                                     |          6 |
| Alimentação      | 9V DC para pedais + 5V DC para eletrônica |          — |

> 💡 A versão anterior utilizava **2 módulos PCF8574**. Nesta versão, o objetivo é utilizar apenas **1 PCF8574**, reduzindo a quantidade de componentes.

---

# 📍 Pinagem — Arduino Mega 2560

| Pino | Função                 | Tipo                  |
| ---: | ---------------------- | --------------------- |
|   22 | Relé — Loop 1          | Saída                 |
|   24 | Relé — Loop 2          | Saída                 |
|   26 | Relé — Loop 3          | Saída                 |
|   28 | Relé — Loop 4          | Saída                 |
|   30 | Relé — Loop 5          | Saída                 |
|   32 | Relé — Mute / Afinador | Saída                 |
|   34 | Botão — Preset 1       | Entrada / Pull-up     |
|   36 | Botão — Preset 2       | Entrada / Pull-up     |
|   38 | Botão — Preset 3       | Entrada / Pull-up     |
|   40 | Botão — Preset 4       | Entrada / Pull-up     |
|   42 | Botão — Preset 5       | Entrada / Pull-up     |
|    2 | Encoder — CLK          | Entrada / Interrupção |
|    3 | Encoder — DT           | Entrada / Interrupção |
|    4 | Encoder — SW           | Entrada / Pull-up     |
|    5 | LED RGB — Vermelho     | Saída                 |
|    6 | LED RGB — Verde        | Saída                 |
|    7 | LED RGB — Azul         | Saída                 |
|    8 | OLED — RST             | Saída                 |
|    9 | OLED — DC              | Saída                 |
|   10 | OLED — CS              | Saída                 |
|   20 | PCF8574 — SDA          | I2C                   |
|   21 | PCF8574 — SCL          | I2C                   |
|   50 | OLED — MISO            | SPI                   |
|   51 | OLED — MOSI            | SPI                   |
|   52 | OLED — SCK             | SPI                   |
|   53 | SPI — SS               | SPI                   |

### SPI do Arduino Mega

Os pinos SPI do Arduino Mega 2560 são:

* **50 — MISO**
* **51 — MOSI**
* **52 — SCK**
* **53 — SS**

O display utiliza o pino **10 como CS**.

---

# 🛠️ Compilação e instalação

## 1. Instale as bibliotecas

O projeto utiliza:

* **Adafruit GFX Library**
* **Adafruit SSD1351**

As bibliotecas podem ser instaladas diretamente pelo gerenciador de bibliotecas da Arduino IDE.

---

## 2. Verifique o endereço I2C

Com os jumpers **A0, A1 e A2 conectados ao GND**, o endereço típico do PCF8574 é:

```cpp
const uint8_t PCF_ADDR = 0x20;
```

Caso esteja utilizando outra configuração de jumpers, altere o endereço em `config.h`.

> ⚠️ Alguns módulos utilizam **PCF8574A**, cuja faixa de endereços é diferente. Se houver dúvidas, utilize um **I2C Scanner** para confirmar o endereço real do módulo.

---

## 3. Verifique a lógica dos relés

O código atualmente assume:

```text
LOW  → Bypass
HIGH → Loop ativo
```

Entretanto, alguns módulos de relé possuem lógica invertida:

```text
LOW  → Relé ativo
HIGH → Relé desligado
```

Se o comportamento estiver invertido, ajuste a função responsável pelo acionamento dos relés em `LoopSwitcher.ino`.

---

## 4. Compile e carregue

1. Coloque todos os arquivos `.ino` e `.h` dentro da pasta:

```text
LoopSwitcher/
```

2. Abra:

```text
LoopSwitcher.ino
```

3. Na Arduino IDE, selecione:

```text
Arduino Mega or Mega 2560
```

4. Selecione a porta serial correspondente.
5. Compile o projeto.
6. Faça o upload para o Arduino.

---

# 🎮 Modos de operação

O sistema possui dois modos principais:

```text
                 ┌─────────────┐
                 │    BOOT     │
                 └──────┬──────┘
                        │
                        ▼
                 ┌─────────────┐
                 │    PLAY     │
                 └──────┬──────┘
                        │
                  Girar encoder
                        │
                        ▼
                 ┌─────────────┐
                 │    CONFIG   │
                 └──────┬──────┘
                        │
                 3x click rápido
                        │
                        ▼
                 ┌─────────────┐
                 │    PLAY     │
                 └─────────────┘
```

---

## 🎸 Modo PLAY — Ao vivo

É o modo principal do sistema e é carregado automaticamente após a inicialização.

| Ação             | Resultado                       |
| ---------------- | ------------------------------- |
| Botão Preset 1–5 | Carrega o preset correspondente |
| Click no encoder | Liga/desliga o Mute             |
| Girar encoder    | Entra no modo CONFIG            |
| 3x click rápido  | Executa reset de fábrica        |

### Estado inicial

Ao ligar o equipamento:

1. A logo é exibida por aproximadamente **2,5 segundos**.
2. O sistema entra automaticamente no modo **PLAY**.
3. O **Loop 1** é ativado.
4. Os demais loops permanecem em bypass.
5. O Mute permanece desligado.

---

# ⚙️ Modo CONFIG — Configuração

O modo CONFIG é acessado girando o encoder enquanto o sistema está em PLAY.

| Ação              | Resultado                          |
| ----------------- | ---------------------------------- |
| Girar encoder     | Navega pelos itens do menu         |
| Girar em `Preset` | Seleciona o preset P1–P5           |
| Long press — 3 s  | Executa a ação do item selecionado |
| 3x click rápido   | Retorna ao modo PLAY               |

A navegação do menu é limitada aos extremos e **não faz loop**.

### Itens do menu

| Item       | Descrição                            |
| ---------- | ------------------------------------ |
| `Preset`   | Seleciona P1–P5 para edição          |
| `Loop 1–5` | Liga/desliga cada loop               |
| `Mute`     | Liga/desliga o Mute                  |
| `SALVAR`   | Salva a configuração atual na EEPROM |
| `VOLTAR`   | Retorna ao modo PLAY                 |

Para alterar os loops ou o Mute, selecione o item e mantenha o encoder pressionado por aproximadamente **3 segundos**.

O preset atualmente selecionado é exibido no canto superior direito da tela.

---

# 💾 Sistema de presets

Cada preset armazena o estado dos:

```text
Loop 1
Loop 2
Loop 3
Loop 4
Loop 5
Mute
```

Os dados são armazenados na **EEPROM interna do Arduino Mega**, permitindo que os presets sejam preservados mesmo após desligar o equipamento.

### Presets disponíveis

```text
P1
P2
P3
P4
P5
```

---

# 🎨 Personalizando a logo

A logo exibida durante o boot pode ser modificada no arquivo:

```text
logo.h
```

Exemplo:

```cpp
const char* BOOT_LOGO[] = {
    " ############# ",
    " ##         ## ",
    " ##  LOOP   ## ",
    " ## SWITCH  ## ",
    " ############# "
};
```

### Recomendações

* Mantenha **21 linhas**, ou ajuste `LOGO_LINES`.
* Utilize aproximadamente **21 caracteres por linha** para melhor centralização.
* Prefira caracteres ASCII simples.
* A logo é exibida durante aproximadamente **2,5 segundos**.

---

# 🚦 LED RGB — Status do sistema

O LED RGB indica o estado geral do equipamento.

| Cor         | Estado   | Significado                  |
| ----------- | -------- | ---------------------------- |
| 🔴 Vermelho | Contínuo | Sistema inicializando        |
| 🟢 Verde    | Contínuo | Sistema pronto / PLAY        |
| 🟡 Amarelo  | Piscando | Mute ativo / afinador ligado |
| 🟡 Amarelo  | Contínuo | Preset sendo salvo           |

---

# 💡 LEDs dos loops — PCF8574

O PCF8574 controla os LEDs indicadores dos loops e do Mute.

| Bit | LED    | Estado                  |
| --- | ------ | ----------------------- |
| P0  | Loop 1 | LED aceso = loop ativo  |
| P1  | Loop 2 | LED aceso = loop ativo  |
| P2  | Loop 3 | LED aceso = loop ativo  |
| P3  | Loop 4 | LED aceso = loop ativo  |
| P4  | Loop 5 | LED aceso = loop ativo  |
| P5  | Mute   | LED aceso = Mute ativo  |
| P6  | Livre  | Reservado para expansão |
| P7  | Livre  | Reservado para expansão |

### ⚠️ Lógica invertida

O PCF8574 trabalha com lógica invertida para os LEDs:

```text
LOW  → LED ligado
HIGH → LED desligado
```

---

# ⚠️ Problemas conhecidos e limitações

Esta versão ainda está em desenvolvimento e **não deve ser considerada uma versão final**.

## 🧪 Hardware real ainda não validado

O código foi desenvolvido e revisado logicamente, porém ainda precisa de testes completos em hardware real.

Podem ocorrer diferenças relacionadas a:

* debounce dos botões;
* comportamento do encoder;
* timing dos gestos;
* acionamento dos relés;
* atualização do display;
* comunicação I2C;
* ruído elétrico.

---

## 🔌 Lógica dos relés

O comportamento depende do módulo utilizado.

Alguns módulos são:

```text
HIGH → ON
LOW  → OFF
```

Enquanto outros utilizam:

```text
LOW  → ON
HIGH → OFF
```

Verifique a documentação do módulo antes de conectar os pedais.

---

## 🔗 Endereço I2C

O endereço padrão esperado é:

```text
0x20
```

Porém, o endereço pode mudar dependendo da configuração dos jumpers ou do modelo do chip.

Exemplo:

```text
PCF8574  → normalmente 0x20–0x27
PCF8574A → normalmente 0x38–0x3F
```

Utilize um I2C Scanner caso o dispositivo não seja detectado.

---

## 🔄 Encoder

A detecção de:

* click;
* triple click;
* long press;
* rotação;

pode exigir calibração dependendo do encoder utilizado.

Caso o triple click não seja reconhecido corretamente, ajuste:

```cpp
TRIPLE_CLICK_WINDOW
```

em `config.h`.

---

## 🖥️ Display

O projeto foi desenvolvido para displays utilizando o controlador:

```text
SSD1351
```

Displays baseados em controladores como:

```text
SSD1306
SH1106
ST7735
```

não são compatíveis diretamente sem alterações no código.

---

## ⚡ Alimentação

Os **6 relés + display OLED + LEDs** podem consumir mais corrente do que é recomendado alimentar diretamente através da USB do Arduino.

Recomenda-se utilizar uma fonte externa adequada, por exemplo:

```text
5V / 1A ou superior
```

A alimentação deve ser dimensionada de acordo com os relés e demais componentes efetivamente utilizados.

> ⚠️ Tenha atenção especial ao **GND comum** entre as fontes e aos requisitos de alimentação dos módulos utilizados.

---

## 💾 EEPROM

A EEPROM do Arduino Mega possui uma vida útil limitada de aproximadamente:

```text
100.000 ciclos de escrita por célula
```

Por isso, evite realizar gravações continuamente dentro de `loop()`.

O projeto foi desenvolvido para salvar presets apenas quando solicitado pelo usuário.

---

## 🔤 Fontes do display

As fontes padrão do Adafruit GFX são relativamente pequenas.

Para utilizar fontes maiores ou personalizadas, será necessário adicionar arquivos de fonte compatíveis com a biblioteca.

---

# 🚀 Próximos passos

Algumas funcionalidades planejadas para versões futuras:

* [ ] Bancos de presets
* [ ] 5 bancos × 5 presets = 25 combinações
* [ ] Backup dos presets via Serial
* [ ] Restauração dos presets via Serial
* [ ] Transições entre presets para reduzir estalos
* [ ] Fade in/out durante mudanças de preset
* [ ] Melhorias na interface do display
* [ ] Configuração mais flexível dos loops
* [ ] Testes completos em hardware real
* [ ] Otimização do consumo e da alimentação

---

# 🧭 Roadmap

### v2.0 — WIP

* [x] Arquitetura modular
* [x] Controle de 5 loops
* [x] Canal Mute
* [x] Presets
* [x] EEPROM
* [x] Display SSD1351
* [x] Encoder
* [x] PCF8574
* [ ] Validação completa em hardware
* [ ] Ajustes de timing
* [ ] Testes de estabilidade
* [ ] Testes com diferentes módulos de relé

---

# 🤝 Contribuições

Sugestões, correções e melhorias são bem-vindas.

Se você encontrar um problema, abra uma **Issue** descrevendo:

1. O comportamento esperado;
2. O comportamento observado;
3. O hardware utilizado;
4. O módulo de relé utilizado;
5. O endereço I2C encontrado;
6. Qualquer mensagem de erro exibida pela IDE ou pelo Serial Monitor.

Pull Requests também são bem-vindos.

---

# ⚠️ Aviso

Este projeto envolve **eletrônica, alimentação elétrica, relés e equipamentos de áudio**.

Monte e teste o circuito com cuidado. Não conecte ou desconecte componentes com o circuito energizado e verifique as tensões de alimentação antes de conectar o hardware.

O autor não se responsabiliza por danos causados ao equipamento, aos pedais ou a outros dispositivos decorrentes do uso deste projeto.

---

## 🎸 Projeto

**Loop Switcher v2.0**

Um projeto experimental para construir um controlador de loops de pedais de guitarra **compacto, programável e totalmente personalizável**.

> *Build it. Play it. Improve it.*
