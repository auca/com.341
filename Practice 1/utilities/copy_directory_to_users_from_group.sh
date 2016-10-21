#!/usr/bin/env bash

if [ "$#" -ne 3 ]; then
    echo "Usage: $0 <GROUP> <DIRECTORY> <RELATIVE DESTINATION>"
fi

GROUP="$1"

SOURCE_DIRECTORY=`dirname "$2"`
TARGET=`basename "$2"`

RELATIVE_DESTINATION_DIRECTORY="$3"

STUDENTS=`getent group "$GROUP" | awk -F ':' '{ print $4 }' | sed 's|,| |g'`

for STUDENT in $STUDENTS
do
    DESTINATION_DIRECTORY="/home/$STUDENT/$RELATIVE_DESTINATION_DIRECTORY"

    rm --recursive --force "$DESTINATION_DIRECTORY/$TARGET"
    mkdir --parents "$DESTINATION_DIRECTORY"

    cp --recursive "$SOURCE_DIRECTORY/$TARGET" "$DESTINATION_DIRECTORY"
    chown --recursive "$STUDENT":"$STUDENT" "$DESTINATION_DIRECTORY"
    chmod --recursive 700 "$DESTINATION_DIRECTORY"
done
