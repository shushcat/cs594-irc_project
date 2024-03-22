#!/usr/bin/bash

TEST_DIR=Testing
NUM_DIRS=5

./create_random_files.bash

if [ $# -gt 0 ]
then
    NUM_DIRS=$1
fi

if [ ! -d ${TEST_DIR} ]
then
    mkdir ${TEST_DIR}
fi
cd ${TEST_DIR}

for D in $(seq ${NUM_DIRS})
do
    if [ ! -d Test${D} ]
    then
        mkdir Test${D}
    fi
    cd Test${D}
    rm -f  C?_zeroes*.dat S_*.dat
    if [ -e ../../rockem_client ]
    then
        ln -sf  ../../rockem_client .
    else
        echo "  Test${D}: link to rockem_client not created, does not exist"
    fi
    for F in ../../zeroes*.dat
    do
	    TEST_FILE=$(basename ${F})
	    cp $F ./C${D}_${TEST_FILE}
    done
    cd ..
    echo "Test${D} done"
done

if [ -e ../rockem_server ]
then
    ln -sf ../rockem_server .
else
    echo "  link to rockem_server not created, does not exist"
fi
rm -f C?_zeroes*.dat S_*.dat
for F in ../random*.dat
do
    TEST_FILE=$(basename ${F})
    cp ${F} ./S_${TEST_FILE}
done
echo "server done"

cd ..
