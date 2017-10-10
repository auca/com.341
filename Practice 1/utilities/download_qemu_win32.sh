#!/usr/bin/env sh

QEMU_ARCHIVE='qemu-2.10.1-win32.tar.gz'
QEMU_DIRECTORY='qemu'
URL="https://dl.dropboxusercontent.com/u/5779716/$QEMU_ARCHIVE"

curl -C - -o "$QEMU_ARCHIVE" "$URL" || wget --continue -o "$QEMU_ARCHIVE" "$URL"

test -e "$QEMU_DIRECTORY" && mv "$QEMU_DIRECTORY" "$QEMU_DIRECTORY"_old
mkdir "$QEMU_DIRECTORY" && tar -xvzf "$QEMU_ARCHIVE" -C "$QEMU_DIRECTORY" --strip-components 1
