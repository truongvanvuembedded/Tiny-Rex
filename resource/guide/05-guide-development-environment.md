
### Development Environment

To ensure a consistent and reproducible build environment, the project is developed using a Docker-based toolchain setup instead of local installation.

## 1. Toolchain in Docker

The Docker image includes:

- GCC ARM Embedded toolchain (arm-none-eabi-gcc)
- GDB multiarch debugger
- Build tools (make)
- Git for source control
- Ak-Flash utility

## 1.1. Build Workflow

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

## 2. Using Dev Container
You can also use a VSCode Dev Container to build and work on this project without installing toolchains locally.
- Build and run container
```bash
Open pallet (Ctrl+Shift+P) -> dev Containers: Reopen in Container
```

## Build project
```bash
make
```

## Flash firmware via st-link
```bash
make flash
```

## Flash firmware via ak-flash
```bash
make flash dev=dev/ttyUSB0
```
