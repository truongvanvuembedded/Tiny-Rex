FROM ubuntu:24.04

RUN apt-get update && \
    apt-get install -y \
    make \
    git \
    wget \
    xz-utils \
    bzip2 \
    ca-certificates \
    build-essential && \
    rm -rf /var/lib/apt/lists/*

# ===== ARM GCC =====
WORKDIR /opt

RUN wget -O gcc.tar.bz2 "https://developer.arm.com/-/media/files/downloads/gnu-rm/10.3-2021.10/gcc-arm-none-eabi-10.3-2021.10-x86_64-linux.tar.bz2?rev=78196d3461ba4c9089a67b5f33edf82a&revision=78196d34-61ba-4c90-89a6-7b5f33edf82a&hash=B94A380A17942218223CD08320496FB1" && \
    ls -lah gcc.tar.bz2 && \
    tar -xjf gcc.tar.bz2 && \
    rm gcc.tar.bz2

ENV GCC_PATH=/opt/gcc-arm-none-eabi-10.3-2021.10
ENV PATH="${GCC_PATH}/bin:${PATH}"

# ===== AK-FLASH TOOL =====
WORKDIR /opt/tools

RUN git clone https://github.com/the-ak-foundation/ak-flash.git

WORKDIR /opt/tools/ak-flash

RUN make && \
    cp build_ak-flash/ak-flash /usr/local/bin/ak-flash

# ===== workspace =====
WORKDIR /workspace

CMD ["/bin/bash"]