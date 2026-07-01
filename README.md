# AK Embedded Base Kit - STM32L151

[<img src="hardware/images/ak-foundation-logo.png" width="240"/>](https://github.com/the-ak-foundation)

This kit would not have been possible without the help of [EPCB](https://epcb.vn/pages/frontpage).

AK Embedded Base Kit, utilizing STM32L151 MCU, is an evaluation kit for advanced embedded software learners.

## Features

- This kit integrates 1.54" Oled LCD, 3 push buttons, and 1 buzzer, which would be sufficient to create a small video game with an event driven paradigm.
- It also includes RS485, Qwiic Connect System, and Grove Ecosystems, suitable for prototyping other practical applications in embedded systems.

[<img src="hardware/images/ak-embedded-base-kit-version-3.jpg" width="480"/>](<https://epcb.vn/products/ak-embedded-base-kit-lap-trinh-nhung-vi-dieu-khien-mcu>)

## Purpose

Students who are enrolled in the AK foundation's embedded training program will make use of this evaluation kit to develop a small unique video game that will be able to run smoothly as well as closely follow an event driven paradigm in embedded systems programming. This repository also contains all the code which would form the AK framework that students can use to facilitate their development process.

We also hope that this repository will also be useful for those are on the look out for a well-built kit to practice their embedded systems programming skills.


[<img src="hardware/images/ak-mcu-kit-hw2-github-1280x640px.png" width="960"/>](<https://epcb.vn/products/ak-embedded-base-kit-lap-trinh-nhung-vi-dieu-khien-mcu>)

## Memory map

AK base kit uses the following memory map to run its application code

- [ 0x08000000 ] : **Boot** [[Tiny-Rex-boot.bin]](https://github.com/ak-embedded-software/ak-base-kit-stm32l151/blob/main/hardware/bin/Tiny-Rex-boot.bin)
- [ 0x08002000 ] : **BSF** [ Memory for data sharing between Boot and Application ]
- [ 0x08003000 ] : **Application** [[Tiny-Rex-application.bin]](https://github.com/ak-embedded-software/ak-base-kit-stm32l151/blob/main/hardware/bin/Tiny-Rex-application.bin)                                             |

>**Note:** After loading the boot and application firmware, you can use [AK - Flash](https://github.com/ak-embedded-software/ak-flash), a CLI to work with the AK base kit, to load the application directly through the kit's USB port. Once installed, the following command will flash user's defined code into the kit's application's memory region.

```sh
ak_flash /dev/ttyUSB0 Tiny-Rex-application.bin 0x08003000
```

## Hardware

[AK base kit's schematic](/hardware/schematic/schematic-ak-embedded-base-kit-version-3.pdf)

[<img src="hardware/images/board-view-top.png" width="480"/>](<https://epcb.vn/products/ak-embedded-base-kit-lap-trinh-nhung-vi-dieu-khien-mcu>)

[<img src="hardware/images/board-view-bottom.png" width="480"/>](https://epcb.vn/products/ak-embedded-base-kit-lap-trinh-nhung-vi-dieu-khien-mcu)

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
## Debug flow
For a step-by-step debugging guide, see:
[Debug Guideline](./debug-guiline.md)

## Reference

| Topic | Link |
| ------ | ------ |
| Tutorials | <https://epcb.vn/blogs/ak-embedded-software> |
| Vendor | <https://epcb.vn/products/ak-embedded-base-kit-lap-trinh-nhung-vi-dieu-khien-mcu> |
