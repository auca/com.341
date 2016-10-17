#!/usr/bin/env sh

EFI='QEMU_EFI.fd'
URL="http://snapshots.linaro.org/components/kernel/leg-virt-tianocore-edk2-upstream/latest/QEMU-AARCH64/RELEASE_GCC5/$EFI"

curl -L -C - -o "$EFI" "$URL" || wget --continue -o "$EFI" "$URL"
