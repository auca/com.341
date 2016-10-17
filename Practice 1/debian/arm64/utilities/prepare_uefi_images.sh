#!/usr/bin/env sh

EFI='QEMU_EFI.fd'
FIRMWARE='flash0.img'
VARIABLES='flash1.img'

test -e "$FIRMWARE" &&
    mv "$FIRMWARE" "$FIRMWARE".old

test -e "$VARIABLES" &&
    mv "$VARIABLES" "$VARIABLES".old

dd if=/dev/zero of="$FIRMWARE" bs=1048576 count=64 &&
    dd if=/dev/zero of="$VARIABLES" bs=1048576 count=64 &&
        dd if="$EFI" of="$FIRMWARE" conv=notrunc
