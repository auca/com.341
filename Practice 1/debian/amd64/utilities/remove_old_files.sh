#!/usr/bin/env sh

EFI='OVMF.fd'
FIRMWARE='flash0.img'
VARIABLES='flash1.img'
DISK='debian-10.1.0-amd64-hd.qcow2'

rm -i "$EFI".old       \
      "$FIRMWARE".old  \
      "$VARIABLES".old \
      "$DISK".old
