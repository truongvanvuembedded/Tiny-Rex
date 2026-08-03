# Tiny-Rex - Game built on AK Embedded Base Kit

<center><img width="3353" height="1890" alt="CODING GAME ZOMWAR" src="resources/images/screens/banner_game_zomwar.gif" />
</center>

<hr>

## Gameplay Demo

<div align="center">
  <video src="https://github.com/user-attachments/assets/1f219060-ba05-4864-b85f-053c3afea595" controls width="480"></video>
</div>

## Documentation

| File | Description |
|---|---|
| [README.md](README.md) | Main project overview, hardware information, gameplay rules, and object descriptions. |
| [docs/01-guide-getting-started.md](docs/01-guide-getting-started.md) | Game programming getting started guide. |
| [docs/02-guide-coding-rules.md](docs/02-guide-coding-rules.md) | Some rules for coding game. |
| [docs/03-design-sequence-object.md](docs/03-design-sequence-object.md) | Runtime sequence diagrams for gameplay objects: Gunner, Bullet, Zombie, Car, Bang, Tombstone, and Border. |
| [docs/04-design-sequence-runtime.md](docs/04-design-sequence-runtime.md) | Runtime signal-processing flow for button input, AK task messages, timers, game-loop ticks, object updates, and Mermaid sequence diagrams. |

## Introduction

### Hardware
- This kit integrates 1.54" Oled LCD, 3 push buttons, and 1 buzzer, which would be sufficient to create a small video game with an event driven paradigm.
- It also includes RS485, Qwiic Connect System, and Grove Ecosystems, suitable for prototyping other practical applications in embedded systems.

[<img src="hardware/images/ak-embedded-base-kit-version-3.jpg" width="480"/>](<https://epcb.vn/products/ak-embedded-base-kit-lap-trinh-nhung-vi-dieu-khien-mcu>)

[AK base kit's schematic](/hardware/schematic/schematic-ak-embedded-base-kit-version-3.pdf)

[<img src="hardware/images/board-view-top.png" width="480"/>](<https://epcb.vn/products/ak-embedded-base-kit-lap-trinh-nhung-vi-dieu-khien-mcu>)

[<img src="hardware/images/board-view-bottom.png" width="480"/>](https://epcb.vn/products/ak-embedded-base-kit-lap-trinh-nhung-vi-dieu-khien-mcu)

## Memory map

AK base kit uses the following memory map to run its application code

- [ 0x08000000 ] : **Boot** [[Tiny-Rex-boot.bin]](https://github.com/ak-embedded-software/ak-base-kit-stm32l151/blob/main/hardware/bin/Tiny-Rex-boot.bin)
- [ 0x08002000 ] : **BSF** [ Memory for data sharing between Boot and Application ]
- [ 0x08003000 ] : **Application** [[Tiny-Rex-application.bin]](https://github.com/ak-embedded-software/ak-base-kit-stm32l151/blob/main/hardware/bin/Tiny-Rex-application.bin)                                             |

>**Note:** After loading the boot and application firmware, you can use [AK - Flash](https://github.com/ak-embedded-software/ak-flash), a CLI to work with the AK base kit, to load the application directly through the kit's USB port. Once installed, the following command will flash user's defined code into the kit's application's memory region.

```sh
ak_flash /dev/ttyUSB0 Tiny-Rex-application.bin 0x08003000
```


## Development Environment

To ensure a consistent and reproducible build environment, the project is developed using a Docker-based toolchain setup instead of local installation.

### Toolchain in Docker

The Docker image includes:

- GCC ARM Embedded toolchain (arm-none-eabi-gcc)
- GDB multiarch debugger
- Build tools (make)
- Git for source control
- Ak-Flash utility

## Build Workflow

```bash
# Build Docker image
docker build -t tiny-rex-game .

# Run container with mounted source code
docker run -it --rm \
    -v $(pwd):/workspace/source \
    --privileged \
    -w /workspace/source \
    tiny-rex-game
```

## Using Dev Container
You can also use a VSCode Dev Container to build and work on this project without installing toolchains locally.
- Build and run container
```bash
Open pallet (Ctrl+Shift+P) -> dev Containers: Reopen in Container
```

# Build project
```bash
make
```

# Flash firmware via st-link
```bash
make flash
```

# Flash firmware via ak-flash
```bash
make flash dev=dev/ttyUSB0
```

## UI Design

- [Lopaka.app](https://lopaka.app/) to create and preview screen layouts.
- [image2cpp](https://javl.github.io/image2cpp/) to scale the image to the exact display size and export it as a bitmap array for the embedded firmware.

## Debug flow
For a step-by-step debugging guide, see:
[Debug Guideline](./debug-guiline.md)
## Basic Game Sequence Logic

```mermaid
sequenceDiagram
    autonumber

    participant Act as Player/<br/>Msg Pool
    participant Scr as Game Screen
    participant Rex as Tiny-Rex Object
    participant Bird as Bird Object
    participant Tree as Tree Object
    participant Line as Line Object
    participant Over_Chk as Over Object

    %%========================
    %% Reset Game
    %%========================
    Act->>Scr: RESET_EVENT
    activate Scr
    Note right of Scr: game_state = IDLE_STATE<br/>current_screen = MENU_SCREEN
    deactivate Scr

    Note over Scr: IDLE_STATE
    Scr->>Rex: TINY_REX_SETUP_EVENT
    Scr->>Bird: BIRD_SETUP_EVENT
    Scr->>Tree: TREE_SETUP_EVENT
    Scr->>Line: LINE_SETUP_EVENT
    Scr->>Over_Chk: OVER_SETUP_EVENT

    loop Every GAME_TICK (50 ms)

        Scr->>Rex: TINY_REX_UPDATE_EVENT
        Scr->>Bird: BIRD_UPDATE_EVENT
        Scr->>Tree: TREE_UPDATE_EVENT
    end

    %%========================
    %% Start Game
    %%========================
    Act->>Scr: PLAY_EVENT
    activate Scr
    Note right of Scr: game_state = PLAY_STATE<br/>current_screen = PLAY_SCREEN
    deactivate Scr

    Note over Scr: PLAY_STATE
    Scr->>Rex: TINY_REX_PLAY_EVENT
    Scr->>Bird: BIRD_PLAY_EVENT
    Scr->>Tree: TREE_PLAY_EVENT
    Scr->>Line: LINE_PLAY_EVENT
    Scr->>Over_Chk: OVER_PLAY_EVENT
    Note over Scr: Create timer 50ms for update

    loop Every GAME_TICK (50 ms)

        Scr->>Rex: TINY_REX_UPDATE_EVENT
        Scr->>Bird: BIRD_UPDATE_EVENT
        Scr->>Tree: TREE_UPDATE_EVENT
        Scr->>Line: LINE_UPDATE_EVENT
        Scr->>Over_Chk: OVER_CHECK_EVENT
        activate Over_Chk
        Note left of Over_Chk: Collision Check
            opt Collision detected
                Over_Chk->>Scr: OVER_CHECK_EVENT
                    activate Scr
                    Note right of Scr: game_state = OVER_STATE
                    Note right of Scr: Delete timer 50ms for update
                    deactivate Scr
            end
        deactivate Over_Chk
    end

    Act->>Scr: BUTTON_UP_PRESS_EVENT
    Scr->>Rex: BUTTON_UP_PRESS_EVENT
    Act->>Scr: BUTTON_DOWN_PRESS_EVENT
    Scr->>Rex: BUTTON_DOWN_PRESS_EVENT
    Act->>Scr: BUTTON_MODE_PRESS_EVENT
    Scr->>Rex: BUTTON_MODE_PRESS_EVENT

    %%========================
    %% Game Over
    %%========================

    Note over Scr: OVER_STATE
    Act->>Scr: BUTTON_MODE_PRESS_EVENT
    activate Scr
    Scr->>Scr: RESET_EVENT
    deactivate Scr

```

## Tiny-Rex Object Sequence

```mermaid
sequenceDiagram
    autonumber

    participant Scr as Game Screen
    participant Rex as Tiny-Rex Object

    %%========================
    %% Setup
    %%========================
    Scr->>Rex: TINY_REX_SETUP_EVENT
    activate Rex
    Note right of Rex: state = RUN_STATE<br/>x = INIT_X<br/>y = INIT_Y<br/>visible = ON<br/>action_image = RUN
    deactivate Rex

    %%========================
    %% Up Button
    %%========================
    Scr->>Rex: BUTTON_UP_PRESS_EVENT
    activate Rex
    Note right of Rex: if state == RUN_STATE<br/>state = JUMP_STATE<br/>speed = JUMP_SPEED
    deactivate Rex

    %%========================
    %% Down Button
    %%========================
    Scr->>Rex: BUTTON_DOWN_PRESS_EVENT
    activate Rex
    Note right of Rex: if state == JUMP_STATE<br/>state = FALL_STATE<br/>speed = FALL_SPEED
    deactivate Rex

    %%========================
    %% Mode Button
    %%========================
    Scr->>Rex: BUTTON_MODE_PRESS_EVENT
    activate Rex
    Note right of Rex: if state == RUN_STATE<br/>state = BEND_OVER_STATE
    deactivate Rex

    %%========================
    %% Release Button
    %%========================
    Scr->>Rex: BUTTON_MODE_RELEASE_EVENT
    activate Rex
    Note right of Rex: if state == BEND_OVER_STATE<br/>state = RUN_STATE
    deactivate Rex

    %%========================
    %% Game Tick
    %%========================
    loop Every GAME_TICK (50 ms)

        Scr->>Rex: TINY_REX_UPDATE_EVENT

        activate Rex

        alt state == RUN_STATE
            Note right of Rex: Run animation
        else state == JUMP_STATE
            Note right of Rex: y -= speed
            opt y <= 0
                Note right of Rex: state = FALL_STATE
            end
        else state == FALL_STATE
            Note right of Rex: y += speed
            opt y >= GROUND_Y
                Note right of Rex: y = GROUND_Y<br/>state = RUN_STATE
            end
        else state == BEND_OVER_STATE
            Note right of Rex: Run animation
        end

        deactivate Rex

    end
```

## Contact & Support

<p style="font-size: 20px;"><strong>TRUONG VAN VU</strong> - Embedded Software Enginneer</p>

``` Note
Thank you for visiting this repository.
If you have any questions, suggestions, or feedback about this project or firmware development, feel free to contact me directly.
```

<a href="https://github.com/truongvanvuembedded">
  <img src="https://img.shields.io/badge/GitHub-TRUONGVANVU-181717?style=for-the-badge&logo=github&logoColor=white"/>
</a>

<a href="https://www.linkedin.com/in/vu-truong-van-1b9291271/">
  <img src="https://img.shields.io/badge/LinkedIn-Truong%20Van%20Vu-0A66C2?style=for-the-badge&logo=linkedin&logoColor=white"/>
</a>
