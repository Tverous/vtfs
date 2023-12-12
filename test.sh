#!/bin/bash

make clean

make

mkdir -p test

sudo insmod vtfs.ko

dd if=/dev/zero of=test.img bs=1M count=50

./mkfs.vtfs test.img

sudo mount -o loop -t vtfs test.img test

