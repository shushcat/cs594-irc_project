#!/bin/sh -e

filesize=$RANDOM
base64 /dev/urandom | head -c "$filesize" > random.tmp
chmod a+r random.tmp

filesize=$RANDOM
base64 /dev/zero | head -c "$filesize" > zeroes.tmp
chmod a+r zeroes.dat
