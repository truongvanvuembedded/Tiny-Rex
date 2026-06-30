FROM ubuntu:24.04

# Update package list and install only required development tools
RUN apt-get update && apt-get install -y \
    # ARM GCC cross compiler for Cortex-M microcontrollers
    gcc-arm-none-eabi \
    \
    # Build tools required to compile projects
    make \
    cmake \
    build-essential \
    \
    # Debugging tool that can connect to ARM targets
    gdb-multiarch \
    \
    # OpenOCD allows flashing/debugging via ST-Link or JTAG
    openocd \
    \
    # ST-Link command line utilities
    stlink-tools \
    \
    # Git is useful to clone repositories
    git \
    \
    # Clang format
    clang-format \
    # Clean package cache to reduce image size
    && rm -rf /var/lib/apt/lists/*

# ===== AK-FLASH TOOL =====
WORKDIR /workspace/tools

RUN git clone https://github.com/the-ak-foundation/ak-flash.git

WORKDIR /workspace/tools/ak-flash

RUN make && \
    cp build_ak-flash/ak-flash /usr/local/bin/ak-flash

# ===== workspace =====
WORKDIR /workspace

CMD ["/bin/bash"]