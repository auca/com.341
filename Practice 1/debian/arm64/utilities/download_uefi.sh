#!/usr/bin/env sh

EFI='QEMU_EFI.fd'
URL="https://dl.dropboxusercontent.com/u/5779716/$EFI"

curl -L -C - -o "$EFI" "$URL" || wget --continue -o "$EFI" "$URL"
