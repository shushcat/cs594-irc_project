#!/bin/sh -ex

IP="192.168.50.58"
PORT="10001"
CCMD="../../rockem_client -uuuuuu -vvv -i ${IP} -p ${PORT}"

cd client

test "$(${CCMD} -c dir | grep "rockem_server_ref*" | sed 's/.* //')" = "rockem_server_ref*"

# Test `get`.
rm -f random1.dat zeroes1.dat
cp ../zeroes1.dat ../server # Copy test files to the server directory,
cp ../random1.dat ../server
${CCMD} -c get random1.dat
test "$(ls -l | grep "random1.dat" | awk '{print $9}')" = "random1.dat"
${CCMD} -c get zeroes1.dat
test "$(ls -l | grep "zeroes1.dat" | awk '{print $9}')" = "zeroes1.dat"
rm random1.dat zeroes1.dat
${CCMD} -c get random1.dat zeroes1.dat
test "$(cmp random1.dat ../random1.dat 2>&1)" = ""
test "$(cmp zeroes1.dat ../zeroes1.dat 2>&1)" = ""

# Test `put`.
cp ../random2.dat .
cp ../zeroes2.dat .
# ${CCMD} -c dir
${CCMD} -c put random2.dat
test "$(${CCMD} -c dir | grep "random2.dat" | awk '{print $9}')" = "random2.dat"
${CCMD} -c put random2.dat zeroes2.dat
test "$(${CCMD} -c dir | grep "random2.dat" | awk '{print $9}')" = "random2.dat"
test "$(${CCMD} -c dir | grep "zeroes2.dat" | awk '{print $9}')" = "zeroes2.dat"

echo "\nPASSED\n"
