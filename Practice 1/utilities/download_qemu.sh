#!/usr/bin/env sh

QEMU_ARCHIVE='qemu-2.7.0-win64.tar.lzma'
QEMU_DIRECTORY='qemu'
URL="http://www.omledom.com/pub/qemu/$QEMU_ARCHIVE"

curl -C - -o "$QEMU_ARCHIVE" "$URL" || wget --continue -o "$QEMU_ARCHIVE" "$URL"

test -e "$QEMU_DIRECTORY" && mv "$QEMU_DIRECTORY" "$QEMU_DIRECTORY"_old
mkdir "$QEMU_DIRECTORY" && tar --lzma -xvf "$QEMU_ARCHIVE" -C "$QEMU_DIRECTORY" --strip-components 1
