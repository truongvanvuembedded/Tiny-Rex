FROM ubuntu:24.04

RUN apt-get update && \
    apt-get install -y \
    make \
    git \
    build-essential \
    gcc-arm-none-eabi \
    gdb-multiarch \
    binutils-arm-none-eabi && \
    rm -rf /var/lib/apt/lists/*

# ===== AK-FLASH TOOL =====
WORKDIR /workspace/tools

RUN git clone https://github.com/the-ak-foundation/ak-flash.git

WORKDIR /workspace/tools/ak-flash

RUN make && \
    cp build_ak-flash/ak-flash /usr/local/bin/ak-flash

# ===== workspace =====
WORKDIR /workspace

CMD ["/bin/bash"]