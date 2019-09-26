#!/usr/bin/env sh

DISK='debian-10.1.0-amd64-hd.qcow2'
SIZE='8G'

test -e "$DISK" && mv "$DISK" "$DISK".old
qemu-img create -f qcow2 "$DISK" "$SIZE"
