# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project overview

Tiny-Rex is a T-Rex/dinosaur-runner game firmware for the **AK Embedded Base Kit** (STM32L151CBT6, Cortex-M3, 128KB flash / 16KB RAM), built on top of "AK" - a custom cooperative, event-driven active-object kernel (tasks + signals, similar in spirit to QP/state machines). This is embedded C/C++ (C99 / C++11), cross-compiled with the `arm-none-eabi` toolchain - there is no host-side test suite; verification happens by building and flashing to real hardware or reasoning about the code.

The app layer has been trimmed down from the original "AK Base Kit" fork to only what the game actually uses: the RF24/UART network stack, Zigbee, Modbus master, the debug command shell, and a handful of other-demo leftovers (unused Arduino-JSON/QRCode/nlohmann libraries, dead display signals for screens that never existed in this game) have all been removed. What remains is the display/screen state machine, the LED "life" heartbeat task, and the Tiny-Rex game objects.

## Build, flash, and debug

All commands are run from the `application/` directory (the `boot/` directory is a separate bootloader project with an identical Makefile structure).

```bash
cd application
make all              # build firmware -> build/Tiny-Rex-application.{axf,bin,elf}
make clean            # remove build/ directory
make info             # print flash/RAM usage for the last build
make flash            # flash via st-link (default)
make flash dev=/dev/ttyUSB0   # flash via ak-flash bootloader instead of st-link
make com dev=/dev/ttyUSB0     # open serial console (minicom, 115200 baud)
make debug            # start openocd + gdb-multiarch using stm32l_init.gdb
make asm              # dump full disassembly to build/*.asm
make sym              # dump symbols sorted by size to build/*.sym
```

Equivalent VS Code tasks exist (`build`, `flash`, `clean`, `debug`, `build-flash`) and a `cortex-debug` launch config (`Tiny-Rex-Debug`) that runs `build-flash` then attaches via OpenOCD/GDB, entry point `main_app`. The project is designed to be opened in the provided Dev Container (`.devcontainer/`), which has the arm-none-eabi toolchain, openocd, and cortex-debug preinstalled and gives the container access to `/dev/bus/usb` for ST-Link.

Flash layout is fixed and matters when changing addresses/linker scripts: bootloader at `0x08000000` (8KB), a BSF shared-data partition at `0x08002000` (4KB), and the application partition at `0x08003000` (116KB). `APP_START_ADDR_VAL` in `application/Makefile` must stay in sync with the linker script and interrupt vector table.

Format code with `clang-format` (config at repo root `.clang-format`: Allman braces, tab indent = 4 cols, no column limit, left-aligned pointers, includes are **not** auto-sorted because their order is meaningful: BSP -> framework -> project):

```bash
clang-format -i path/to/file.cpp
find application/sources/app -type f \( -name "*.cpp" -o -name "*.h" \) -not -path "*/libraries/*" -exec clang-format -i {} +
```

## Architecture

### The AK active-object kernel

Everything runs as cooperative **tasks** that communicate exclusively through **signals** carried in messages (`ak_msg_t`), dispatched by a priority scheduler (`application/sources/ak/`, core headers in `ak/inc/`: `ak.h`, `task.h`, `message.h`, `timer.h`, `fsm.h`). There is no shared-memory RPC between modules - a task never calls another task's internals directly; it posts a signal and the target task's handler reacts on its next scheduling pass. The kernel itself is treated as a vendored, reusable engine (shared conceptually with `boot/` and the upstream AK Embedded Base Kit) and is not renamed to game-specific naming - only the app layer built on top of it is.

Signal ID ranges are namespaced: `AK_SYS_DEFINE_SIG` (system), `AK_USER_DEFINE_SIG` (app framework), `TINY_REX_DEFINE_SIG` (game-specific), each anchoring an enum block in a header. `AK_TASK_EOT_ID`/`AK_TASK_POLLING_EOT_ID` are sentinel values the kernel (`ak/src/task.c`) checks by name to find the end of `app_task_table`/`app_task_polling_table` - don't rename or reorder these.

Task IDs are registered centrally in `application/sources/app/task_list.h`/`.cpp` (`app_task_table`), each bound to a priority level (`TASK_PRI_LEVEL_0..7`) and a handler entry point. When adding a task: add its ID to the enum in `task_list.h` (IDs must stay in increasing order), declare its handler, and register it in `task_list.cpp`. App-framework task IDs and signals use a `TINY_REX_` prefix (e.g. `TINY_REX_TASK_DISPLAY_ID`, `TINY_REX_LIFE_SYSTEM_CHECK`) rather than the upstream base kit's generic `AC_`/`GW_` naming.

### App layer (`application/sources/app/`)

- `app.cpp/.h` - app entry/init, top-level signal enum anchors. `sys_irq_shell()` in `app.cpp` still services the console UART's RX interrupt (must read the data register to clear `USART_IT_RXNE`) even though there's no command shell consuming the byte anymore.
- `task_display.cpp/.h` - owns the OLED screen state machine; dispatches button/timer signals to the active screen.
- `screens/` - one `scr_*` module per UI screen (`scr_startup`, `scr_menu`, `scr_idle`, `scr_play`, `scr_ranking`, `scr_setting`) plus shared `screens_bitmap.*` bitmap data. `scr_idle` is the attract-mode screen opened by Exit in the menu (the T-Rex jumps trees by itself, any button returns to the menu). `scr_play` is the game screen - it drives the gameplay loop and fans signals out to the game-object tasks (see the sequence diagram in `resource/guide/03-design-sequence-object.md`).
- `tiny_rex_game/` - the actual game objects, each its own AK task: `tiny_rex_object` (player: RUNNING/JUMPING/FALLING/DUCKING state machine), `obstacles_object` (spawn/move/collision), `horizon_object` (scrolling background), `score` (score + difficulty ramp-up, broadcasts speed-increase signals to the other objects when score thresholds are crossed).
- `app_eeprom.*`, `app_non_clear_ram.*`, `app_flash.h` - persistence for high scores / settings across resets.
- `task_life.cpp/.h` - LED heartbeat task (`TINY_REX_TASK_LIFE_ID`), unrelated to gameplay but kept as generic hardware bring-up plumbing.

### Other source areas (`application/sources/`)

- `driver/` - hardware drivers (OLED via `Adafruit_oled_drv`, `button`, `buzzer`, `eeprom`, `flash`, `gpio`, `led`).
- `common/` - shared utilities: `screen_manager`/`view_render`/`view_item` (screen/view abstraction used by `screens/`), `container/` (fifo, ring buffer, log queue), `xprintf`, `utils`.
- `platform/stm32l/` - STM32L1 CMSIS/StdPeriph HAL, startup code, and the Arduino compatibility layer (`platform/stm32l/arduino/`, used for `SPI`/`WString`/`HardwareSerial`).

There is no longer a `networks/` or `libraries/` directory: the RF24 wireless stack, the UART "link" gateway protocol, Zigbee (`ArduinoZigBee`), Modbus master (`mbmaster`), and the unused `ArduinoJson`/`QRCode`/`nlohmann` vendored libraries were all removed as unused-by-the-game leftovers from the original fork, along with their app-layer glue (`task_if`, `task_rf24_if`, `task_uart_if`, `task_zigbee`, `app_modbus_pull`, the firmware-over-network task `task_fw`, and the debug command shell `task_shell`/`shell.cpp`/`common/cmd_line.*`). Each subdirectory that remains has its own `Makefile.mk`, explicitly listing sources (not globbed), included by the top-level `application/Makefile`; adding a new source file means adding it to the relevant `Makefile.mk`.

## Coding conventions

Full detail lives in `resource/guide/02-guide-coding-rules.md`; the essentials:

- **Folders/files/variables/functions:** `lower_snake_case`. Source/header files carry a module prefix (e.g. `zw_game_*`, `scr_*`) so grepping the prefix returns every symbol/file of that module.
- **Macros, signal enum values, task IDs:** `UPPER_SNAKE_CASE`. A macro belonging to an object must be prefixed with that object's name (`<OBJECT>_<PROPERTY>`, e.g. `BULLET_NUMBER`, `GUNNER_AXIS_X`), never a bare magic number. Game-wide (non-object) macros use the game's project prefix (e.g. `ZW_GAME_TIME_TICK_INTERVAL`, `TINY_REX_*` for the app framework).
- **Signals** are the public contract between tasks - always spelled out in full (`<GAME>_<OBJECT>_<ACTION>`), never abbreviated, including in comments/docs/diagrams.
- **Header guards:** `__<FILE_NAME>_H__` matching the filename exactly.
- **Types:** `lower_snake_case` + `_t` suffix, anonymous struct behind a typedef.
- Cross-module globals: `extern` in the header, defined once in the owning `.cpp`. Module-internal state: `static` in the `.cpp`, never stashed inside another module's file.
- **Commit messages:** `[ACTION] short description`, tag uppercase, description lowercase imperative, no trailing period, ~70 char target. Tags: `[ADD]`, `[UPDATE]`, `[FIX]`, `[REMOVE]`, `[DOC]`, `[MERGE]`.
- **Docs:** files under `resource/guide/` follow `<NN>-<category>-<topic>.md` in `kebab-case`; rename with `git mv` to preserve history.

## Key docs

| File | Contents |
|---|---|
| `README.md` | Hardware overview, MCU/flash layout, gameplay rules, top-level sequence diagram. |
| `resource/guide/01-guide-getting-started.md` | Fork/clone workflow, environment setup, walkthrough of adding a new screen. |
| `resource/guide/02-guide-coding-rules.md` | Full naming/style/commit conventions (source of the summary above). |
| `resource/guide/03-design-sequence-object.md` | Per-object (T-Rex, Obstacle, Horizon) signal sequence diagrams - read before touching `tiny_rex_game/`. |
| `resource/guide/05-guide-development-environment.md`, `06-guide-debug.md` | ST-Link + VS Code + Cortex-Debug setup/debug walkthrough. |
