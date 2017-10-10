#!/usr/bin/env sh

EFI='QEMU_EFI.fd'
URL="https://www.dropbox.com/s/h6dgdrsglagcir0/$EFI?dl=1"

test -e "$EFI" && mv "$EFI" "$EFI".old
curl --location -C - --output "$EFI" "$URL" || wget --continue --output-file="$EFI" "$URL"
