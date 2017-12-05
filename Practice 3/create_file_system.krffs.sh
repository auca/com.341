#!/usr/bin/env sh

#
# create_file_system.krffs.sh
#
# Creates an empty file system file of a specified size and initializes a KRFFS
# file system on it.
#
#     Usage:
#          ./create_file_system.krffs.sh [size in megabytes]
#
# Without the size parameter, the file size is set to 10 megabytes.
#

FILE_SYSTEM_FILE="file_system.krffs"
BLOCK_SIZE="1048576"
COUNT=${1:-10}

rm -i "$FILE_SYSTEM_FILE"
dd if=/dev/zero of="$FILE_SYSTEM_FILE" bs=$BLOCK_SIZE count=$COUNT
./mkfs.krffs "$FILE_SYSTEM_FILE"

