#!/usr/bin/env sh

FIRMWARE='flash0.img'
VARIABLES='flash1.img'
DISK='debian-8.6.0-arm64-hd.qcow2'

rm -i "$FIRMWARE".old  \
      "$VARIABLES".old \
      "$DISK".old
