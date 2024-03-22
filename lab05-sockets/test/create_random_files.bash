#!/bin/bash

RAND_FILE=random
ZER_FILE=zeroes

filesize=$RANDOM
base64 /dev/urandom | head -c "$filesize" > ${RAND_FILE}${filecount}.dat
chmod a+r ${RAND_FILE}${filecount}.dat

filesize=$RANDOM
filesize=$((filesize+512*filecount))
base64 /dev/zero | head -c "$filesize" > ${ZER_FILE}${filecount}.dat
chmod a+r ${ZER_FILE}${filecount}.dat
