#!/bin/bash

if [ -e .create_files_done ]
then
    echo "*** files already created ***"
    echo "*** if you want to force recreation, 'rm .create_files_done' ***"
    echo "*** then, rerun this script ***"
    exit 0
fi

NUM_FILES=10
if [ $# -gt 0 ]
then
    NUM_FILES=$1
fi

RAND_FILE=random
ZER_FILE=zeroes

for filecount in $(seq ${NUM_FILES})
do
    filesize=$RANDOM
    filesize=$((filesize+768*filecount))
    base64 /dev/urandom | head -c "$filesize" > ${RAND_FILE}${filecount}.dat
    chmod a+r ${RAND_FILE}${filecount}.dat

    filesize=$RANDOM
    filesize=$((filesize+512*filecount))
    base64 /dev/zero | head -c "$filesize" > ${ZER_FILE}${filecount}.dat
    chmod a+r ${ZER_FILE}${filecount}.dat
done

filesize=5000000
base64 /dev/urandom | head -c "$filesize" > random_big.dat
chmod a+r random_big.dat

filesize=75000000
base64 /dev/zero | head -c "$filesize" > zeroes_big.dat
chmod a+r zeroes_big.dat


touch .create_files_done
echo "*** done ***"
