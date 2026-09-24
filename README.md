# CHIP-8 Emulator

#### Video Demo: [https://www.youtube.com/watch?v=XlR-y0D-5jI]

```text
  +-----------------------------------------------------------------------+
  |  [o] [o] [o]           CHIP-8 Virtual Machine (64x32)          [ _ ][X] |
  +-----------------------------------------------------------------------+
  |                                                                       |
  |             |                                   |                     |
  |             |                ■■■■               |                     |
  |             |                  ■■               |                     |
  |             |                                   |                     |
  |             |                                   |   DELAY: 00         |
  |             |                                   |   SOUND: 00         |
  |             |                                   |                     |
  |             |                                   |   REGISTERS:        |
  |             |        ■■    ■■■■                 |   V0: 0x00          |
  |             |      ■■■■    ■■■■                 |   VF: 0x00          |
  |             |    ■■■■■■  ■■■■■■■■  ■■■■         |   PC: 0x200         |
  |             +-----------------------------------+   SP: 0x00          |
  |                                                                       |
  |   STATUS: RUNNING          CLOCK: 900 Hz          MODE: MODERN/LEGACY |
  +-----------------------------------------------------------------------+
```

#### Description:

This project is a complete virtual machine and hardware emulator for the classic 1970s CHIP-8 interpreted programming system, implemented from scratch in C using SDL3. It faithfully replicates the execution environment of early microcomputers such as the COSMAC VIP and Telmac 1800, while also accommodating modern Super-CHIP and contemporary interpreter conventions through configurable legacy quirk modes.

The emulator reads compiled binary ROM images into an internal memory buffer, runs a cycle-accurate fetch-decode-execute CPU loop, decodes 35 distinct opcodes, renders a 64x32 monochrome display matrix via texture streaming, and maps modern keyboard events to the original 16-key hexadecimal keypad.

---

### Project Structure & File Walkthrough

The codebase is organized into modular units separating architectural definitions, peripheral drivers, and the central execution engine:

#### 1. `helper.h`

The header file establishes the structural foundation and constants of the CHIP-8 system.

- **Core Definitions:** Defines critical memory constants, including the total 4096-byte address space (`MEMORY_SIZE_BYTES`), base resolution dimensions (`64x32`), program counter starting address (`0x200`), and built-in hexadecimal font offset boundaries (`0x50`–`0x9F`).
- **`CHIP8` Structure:** Encapsulates the entire machine state within a single composite type. This includes 4 KB of memory, 16 8-bit general registers (`V0` through `VF`), a 16-bit address index register (`I`), the program counter (`pc`), a 16-level subroutine call stack with an associated stack pointer (`sp`), 60 Hz delay and sound timer registers, the 2D boolean screen array, and the 16-button keypad state array. It also maintains flags for legacy execution modes, draw triggers, and blocking key-release states.
- **`GraphicsContext` Structure:** Packages SDL3 windowing, renderer pointers, and streaming texture handles together with an initialization success flag.

#### 2. `helper.c`

This file implements subsystem setup, ROM loading, input polling, and graphics presentation.

- **Fontset & Keymap:** Stores the 80-byte default font set representing hexadecimal glyphs (`0` through `F`) in 5-byte sprite patterns. It also defines the scancode lookup table `KEYPAD_MAPPING` that translates a modern 4x4 keyboard cluster (`1-4`, `Q-R`, `A-F`, `Z-V`) into native CHIP-8 hex keypad codes (`0x0` through `0xF`).
- **`set_up_SDL`:** Initializes SDL3 video subsystems, creates a resizable application window and renderer, and generates an `SDL_PIXELFORMAT_RGBA8888` streaming texture. Crucially, it invokes `SDL_SetRenderLogicalPresentation` with `SDL_LOGICAL_PRESENTATION_LETTERBOX` and applies `SDL_SCALEMODE_NEAREST` to guarantee that scaling to modern high-resolution displays retains sharp, pixelated borders without stretching or aspect-ratio distortion.
- **`load_ROM`:** Opens the targeted binary file in read-binary mode (`"rb"`), reads 2-byte instructions sequentially directly into the virtual machine's RAM starting at address `0x200`, and enforces a memory safety check ensuring the program binary does not overflow available capacity.
- **`draw_graphics`:** Direct-locks the streaming texture via `SDL_LockTexture`, iterates across the 64x32 boolean display grid, writes 32-bit color values (`0xFFFFFFFF` for active pixels and `0x000000FF` for inactive background pixels) row by row respecting pitch offsets, and presents the rendered frame to the screen.
- **`handle_input` & `update_timers`:** Polls active keyboard states across the 16 mapped scancodes each frame and decrements the hardware delay and sound timers at 60 Hz.

#### 3. `chip8_emulator.c`

The central driver containing `main` and the core instruction execution loop.

- **Main Loop & Timing:** Paces execution to 60 frames per second using `SDL_GetTicks`. Within each 16.6 ms frame interval, it runs 15 CPU cycles (`CYCLES_PER_FRAME`), resulting in an effective execution clock of 900 Hz[cite: 1]. A deliberate `SDL_Delay(1)` is included to yield CPU control back to the operating system, preventing unnecessary power consumption and thread spinning.
- **`emulate_cycle`:** Fetches the 16-bit big-endian opcode from memory, increments `pc` by two bytes, decodes the high nibble, and dispatches the instruction through a multi-level `switch` structure. It handles arithmetic operations, bitwise logic, memory operations, jumps, random number masking, and sprite drawing with XOR pixel blending and collision detection on register `VF`.

---

### Design Decisions & Implementation Debates

During development, several architecture and implementation alternatives were evaluated:

1. **Configurable Legacy Quirks via Command-Line Argument:**
   CHIP-8 software spans multiple eras. The original 1977 COSMAC VIP implementation shifted register `Vy` and stored the result in `Vx` for opcodes `8XY6` and `8XYE`, whereas modern Super-CHIP interpreters shift `Vx` in place. Similarly, original VIP interpreters modified the index register `I` when saving or loading registers via `FX55` and `FX65`, while newer implementations leave `I` unchanged. Rather than hardcoding one standard, the emulator implements an optional command-line flag (`[is_legacy=0|1]`). When enabled, instructions adhere strictly to COSMAC VIP behavior (including clearing `VF` on bitwise AND/OR/XOR operations and modifying `I`), allowing older games and modern ROMs to run with accurate behavior.

2. **Decoupled Pacing vs. Cycle Throttling:**
   Early prototypes stepped instructions without frame-time synchronization, resulting in unplayable execution speeds on modern processors. The loop was redesigned to bind frame pacing strictly to 60 Hz ticks while grouping instructions into fixed bursts (15 instructions per frame). This maintains independent 60 Hz timer updates and smooth input polling while granting predictable CPU clock rates.

3. **SDL3 Streaming Textures with Nearest-Neighbor Scaling:**
   Drawing individual rectangles with `SDL_RenderFillRect` creates excessive rendering overhead. By locking a streaming texture in `RGBA8888` format, writing the buffer directly, and delegating the scale transformation to SDL3's GPU presentation pipeline with nearest-neighbor interpolation, rendering remains hardware-accelerated, crisp, and performant.

---

### Building and Running

#### Prerequisites

- A C compiler supporting C17/C23 (e.g., `gcc` or `clang`)
- SDL3 development libraries installed on your operating system

#### Compilation

Compile using `gcc`:

```bash
gcc *.c -o ./chip8_emulator.exe -lSDL3
```

Compile using `make` (make sure you have `make` installed):

```bash
make chip8
```

#### Debug

For debug compile program using `gcc`:

```bash
gcc -g3 -O0 -Wall -Wextra *.c -o ./chip8_emulator.exe
```

Or `make`:

```bash
make debug
```

---

### Contributions & Acknowledgments

This emulator was developed independently by **Levan Demetrashvili** as the final project for **CS50x** (Harvard University's Introduction to Computer Science).

Special thanks and acknowledgments to:

- **[The CS50 Staff](https://cs50.harvard.edu/x/):** For providing an exceptional computer science foundation and curriculum.
- **[Cowgod's CHIP-8 Technical Reference](http://devernay.free.fr/hacks/chip8/C8TECH10.HTM):** For comprehensive hardware documentation of the original COSMAC VIP architecture and opcode specifications.
- **[Timendus & the CHIP-8 Community](https://github.com/Timendus/chip8-test-suite):** For providing publicly available open-source test ROMs and test suites that proved invaluable for debugging instruction timing, flag operations, and quirk behavior.
