#!/usr/bin/env sh

DEBIAN='debian-10.1.0-arm64-netinst.iso'
URL="http://mirrors.kernel.org/debian-cd/10.1.0/arm64/iso-cd/$DEBIAN"

curl -C - -o "$DEBIAN" "$URL" || wget --continue -o "$DEBIAN" "$URL"
