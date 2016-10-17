#!/usr/bin/env sh

qemu-system-aarch64                                        \
    -machine virt                                          \
    -cpu cortex-a57                                        \
    -m 256M                                                \
    -smp 1                                                 \
    -nographic                                             \
    -drive if=pflash,file=flash0.img,format=raw,readonly   \
    -drive if=pflash,file=flash1.img,format=raw,readonly   \
    -drive if=none,file=debian-8.6.0-arm64-hd.qcow2,id=hd0 \
    -device virtio-blk-device,drive=hd0                    \
    -netdev user,hostfwd=tcp::2222-:22,id=eth0             \
    -device virtio-net-device,netdev=eth0                  \
    -loadvm last
