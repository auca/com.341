#!/usr/bin/env sh

EFI='OVMF.fd'
URL='http://sourceforge.net/projects/edk2/files/OVMF/OVMF-X64-r15214.zip/download'

test -e "$EFI" && mv "$EFI" "$EFI".old
curl -o "$EFI".zip "$URL" || wget -o "$EFI".zip "$URL"
