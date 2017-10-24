#!/usr/bin/env sh

QEMU_ARCHIVE='qemu-2.10.1-win32.tar.gz'
QEMU_DIRECTORY='qemu'
URL="https://www.dropbox.com/s/651b2rpunco4hwy/$QEMU_ARCHIVE?dl=1/"

curl --location -C - --output "$QEMU_ARCHIVE" "$URL" || wget --continue --output-file="$QEMU_ARCHIVE" "$URL"

test -e "$QEMU_DIRECTORY" && mv "$QEMU_DIRECTORY" "$QEMU_DIRECTORY"_old
mkdir "$QEMU_DIRECTORY" && tar -xvzf "$QEMU_ARCHIVE" -C "$QEMU_DIRECTORY" --strip-components 1
