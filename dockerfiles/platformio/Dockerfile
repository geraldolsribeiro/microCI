FROM python:3.15.0b4-slim-trixie
LABEL org.opencontainers.image.authors="Geraldo Luis da Silva Ribeiro <geraldolsribeiro@gmail.com>"

# See latest version at https://github.com/platformio/platformio-core/releases
ARG PLATFORMIO_VERSION=6.1.19

RUN apt-get update \
  && apt-get install -y --no-install-recommends \
    git \
    dfu-util \
    libusb-1.0-0 \
  && pip install --no-cache-dir "platformio==${PLATFORMIO_VERSION}" \
  && rm -rf /var/lib/apt/lists/*

RUN apt-get update \
  && apt-get install -y --no-install-recommends \
    gcc-riscv64-linux-gnu \
    gcc-riscv64-unknown-elf \
    g++-riscv64-linux-gnu \
  && rm -rf /var/lib/apt/lists/*

RUN pio pkg install --global --platform nuclei
RUN pio pkg install --global --tool "platformio/tool-scons"
RUN pio pkg install --global --tool nuclei/framework-nuclei-sdk
RUN pio pkg install --global --tool nuclei/toolchain-riscv-gcc-nuclei

WORKDIR /microci_workspace 

RUN useradd -ms /bin/bash pio && \
    mkdir -p /home/pio/.platformio && \
    chown -R pio:pio /home/pio

ENV HOME=/home/pio

# Set environment variables for PlatformIO core path & home
ENV PLATFORMIO_DISABLE_UPGRADE_CHECK=true
# ENV PLATFORMIO_CORE_DIR=/opt/platformio
ENV PLATFORMIO_CORE_DIR=/home/pio/.platformio

USER pio

# Disable automatic updates for platforms and libraries
# Maximize the check intervals (forces it to wait 999 days between checks)
RUN echo "Offline mode" > /dev/null \
   && pio settings set check_platformio_interval 999 \
   && pio settings set enable_telemetry No \
   && pio settings set enable_cache No \
   && pio settings set force_verbose Yes \
   && pio settings set projects_dir /microci_workspace

RUN mkdir -p /tmp/test \
  && pio project init --project-dir /tmp/test --board gd32vf103v_eval



#   && pio settings set auto_update_platforms No \
#   && pio settings set auto_update_libraries No \
#   && pio settings set check_platforms_interval 999 \
#   && pio settings set check_libraries_interval 999


CMD ["/bin/bash" ]

# https://www.youtube.com/@ChandlerKluser/search?query=longan

# https://sigmdel.ca/michel/ha/gd32v/longan_nano_01_en.html

# https://github.com/smuehlst/longan-nano-experiments

# https://www.appelsiini.net/2020/programming-gd32v-longan-nano/

# https://sourceforge.net/projects/platformio-storage/files/packages/
# toolchain-gd32v
# tool-openocd-gd32v
# tool-gd32vflash

# The toolchain-gd32v package is the RISC-V GCC C/C++ compiler toolchain used by
# PlatformIO and other build systems to compile firmware for GigaDevice GD32V
# RISC-V microcontrollers (such as the GD32VF103 used on the Sipeed Longan Nano).

# https://community.platformio.org/t/i-am-unable-to-install-sipeed-longan-nano-gd32v/34758/8
# Not an expert here, but I found switching to the “nuclei-sdk” framework instead
# of the default “gd32vf103-sdk”, along with “platform” and “board”, bypasses the
# issue. Here is my platformio.ini entry
#
# [env:gd32vf103c_longan_nano]
# platform = nuclei
# board = gd32vf103c_longan_nano
# framework = nuclei-sdk
# debug_tool = rv-link

