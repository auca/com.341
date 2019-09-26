#!/usr/bin/env sh

DISK='debian-10.1.0-arm64-hd.qcow2'

test -e "$DISK" && mv "$DISK" "$DISK".old
qemu-img convert -O qcow2 -c "$DISK".old "$DISK"
