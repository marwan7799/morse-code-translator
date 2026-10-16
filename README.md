# 🤖 Morse Code Translator

**Type it. Tap it. Hear it. See it.**
A dual-mode embedded Morse code tool: translate text into Morse (buzzer + LEDs), or tap Morse on a keypad and watch it decode on an LCD.

Built during our college **summer training** program, simulated in **Proteus** and programmed in **C/C++** with **PlatformIO**.

[Simulation](simulation/) · [Source code](src/main.cpp) · [Report a bug](../../issues/new)

---

## 🎯 What it does

The device has two modes, toggled at any time with the **M** key:

| Mode | Direction | What you do | What you get |
|------|-----------|-------------|--------------|
| **0 – Morse → Text** | Morse to letters | Tap dots, dashes and a "decode" key on the keypad | The decoded message appears on the LCD |
| **1 – Text → Morse** | Letters to Morse | Type a word on the keypad and press "send" | The buzzer beeps and the LEDs flash the Morse code, letter by letter |

## ✨ Features

- 🔁 **Two-way translation**: Morse → text and text → Morse in one device
- 🔊 **Audio + visual feedback**: buzzer for every signal, with color-coded LEDs
- 🔴 **Red** = dot · 🟢 **Green** = dash · 🔵 **Blue** = letter decoded / message sent
- 📟 **16×2 LCD** shows the current code, decoded message, and playback status (`Playing: A .-`)
- ⌨️ **4×4 matrix keypad** with software debouncing
- 🔤 **Full lookup table**: A–Z and 0–9 (36 characters)
- 🧹 **Clear key** to reset the buffers, and automatic message wrap when the screen is full

## 🧰 Hardware

| Component | Purpose |
|-----------|---------|
| ATmega microcontroller (16 MHz) | Runs the firmware |
| 16×2 character LCD (HD44780) | User interface |
| 4×4 matrix keypad | Input |
| RGB LEDs (or 3 separate LEDs) | Dot / dash / status indicators |
| Buzzer | Audio output |
| Resistors, breadboard / Proteus | Wiring and simulation |

### 🔌 Pin map

| Signal | Pin |
|--------|-----|
| LCD (RS, EN, D4–D7) | `PA1`, `PA2`, `PA3`, `PA4`, `PA5`, `PA6` |
| Keypad rows (outputs) | `PB4` – `PB7` |
| Keypad columns (inputs, pull-up) | `PD2` – `PD5` |
| Red LED | `PC0` |
| Green LED | `PC1` |
| Blue LED | `PC2` |
| Buzzer | `PC5` |

## ⌨️ Keypad layout & controls

```
 ┌───┬───┬───┬───┐
 │ A │ E │ I │ M │   M = toggle mode
 ├───┼───┼───┼───┤
 │ 1 │ F │ J │ * │   * = dot
 ├───┼───┼───┼───┤
 │ < │ 2 │ K │ - │   - = dash
 ├───┼───┼───┼───┤
 │ C │ Y │ N │ + │   + = decode / send
 └───┴───┴───┴───┘
```

| Key | Morse → Text mode | Text → Morse mode |
|-----|-------------------|-------------------|
| `*` | Add a **dot** (`.`) | – |
| `-` | Add a **dash** (`-`) | – |
| `+` | **Decode** the current code into a letter | **Play** the typed text as Morse |
| `1` | Insert a **space** in the message | – |
| `<` | **Clear** everything | **Clear** the typed text |
| `M` | Switch mode | Switch mode |
| Letters / digits | – | Appended to the text buffer |

## 🚀 How to use

### Morse → Text
1. Tap `*` and `-` to build one letter (e.g. `.-`). Each press beeps and lights an LED.
2. Press `+` to decode it. The letter is added to **Msg:** and the blue LED blinks.
3. Press `1` for a space between words, `<` to start over.

### Text → Morse
1. Press `M` to switch modes (LCD shows `Mode: Txt->Morse`).
2. Type your text using the keypad letters.
3. Press `+`. Each character is shown on the LCD while the buzzer and LEDs play its Morse code.
4. The LCD shows `Morse: Sent!` when finished. Press `<` to clear.

## ⏱️ Timing

| Element | Duration |
|---------|----------|
| Dot | 150 ms |
| Dash | 400 ms |
| Gap between signals | 150 ms |
| Gap between letters | 450 ms |

## ⚙️ Limits

| Setting | Value |
|---------|-------|
| Max dots/dashes per letter (`MORSE_MAX`) | 7 |
| Max decoded message length (`MSG_MAX`) | 16 (wraps when full) |
| Max typed text (`TEXT_MAX`) | 12 |

## 📋 Requirements

| Tool | Used for | Link |
|------|----------|------|
| **Visual Studio Code** | Code editor | https://code.visualstudio.com |
| **PlatformIO IDE** (VS Code extension) | Compiling the C/C++ code into a `.hex` file | Search **"PlatformIO IDE"** in the VS Code Extensions tab, or https://platformio.org/install/ide?install=vscode |
| **Proteus** (Design Suite 8.x) | Simulating the circuit | https://www.labcenter.com |

> PlatformIO downloads the AVR toolchain and the `LiquidCrystal` library automatically the first time you build, so an internet connection is needed once.

## 🛠️ Build & run

### 1. Get the project
```bash
git clone https://github.com/malakhub/morse-code-translator.git
```
Open the project folder in VS Code. PlatformIO detects `platformio.ini` automatically.

### 2. Build the firmware
- Click the **✔ Build** button in the blue PlatformIO bar at the bottom of VS Code, or run:
  ```bash
  pio run
  ```
- When it finishes with `SUCCESS`, the compiled program is here:
  ```
  .pio/build/<env>/firmware.hex
  ```
  (`<env>` is the environment name written in `platformio.ini`.)

### 3. Load the program into the ATmega in Proteus
1. Open `simulation/morse-code-translator.pdsprj` in Proteus.
2. **Double-click the ATmega IC** to open its *Edit Component* window.
3. Next to **Program File**, click the 📁 folder icon and select the `firmware.hex` file from step 2.
4. Set **Clock Frequency** to `16MHz` (the code assumes `F_CPU = 16000000`).
5. Click **OK**, then press ▶ **Play** at the bottom-left.
6. Click the keypad buttons in the simulation to use the device.

> 🔁 **Every time you change the code:** rebuild in VS Code, then press Play again in Proteus (or re-select the `.hex` file if Proteus doesn't pick up the change).

### Flashing a real board (optional)
```bash
pio run -t upload
```

## 🗂️ Project structure

```
morse-code-translator/
├── README.md
├── platformio.ini            # PlatformIO build config
├── src/
│   └── main.cpp              # Firmware (keypad, LCD, Morse logic)
├── include/  lib/  test/     # PlatformIO defaults
├── simulation/
│   └── morse-code-translator.pdsprj   # Proteus project
└── docs/
    ├── schematic.png         # Circuit diagram
    └── demo.gif              # Short demo of both modes
```

## 🧠 How it works

- **Keypad scan:** rows (`PB4–PB7`) are driven low one at a time while the columns (`PD2–PD5`) are read; a 20 ms delay debounces each press.
- **Lookup table:** a `MorseEntry { code, letter }` array maps all 36 characters. `decode_morse()` and `encode_morse()` search it in both directions.
- **Output:** `play_morse()` turns the LEDs and buzzer on for the length of each dot or dash, then off for the gap.
- **Bit-manipulation macros** (`SET_BIT`, `CLR_BIT`, `GET_BIT`, `TOG_BIT`) are used for direct register access to the I/O ports.

## ⚠️ Known limitations & ideas

- Keypad has only a subset of letters, so not every character can be typed in Text → Morse mode.
- Delays are blocking (`_delay_ms`), so keys can't be read during playback.
- Dot/dash timing isn't the strict 1:3 ratio of standard Morse.
- Ideas: timer-based non-blocking playback, a full alphanumeric keypad or a single push-button with press-length detection, adjustable speed (WPM), ESP32 port with Wi-Fi/Bluetooth.

## 👥 Team

Made by *Malak Medhat / Marwan Mohamed / Omer Fekry / Habiba Ahmed / Esmail Mohamed * during the *Faculty of Engineering Capital University* summer training, *2026*.
