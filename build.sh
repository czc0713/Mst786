#! /bin/bash
export PATH=/disk/project/mst786/src/tools/gcc-linaro-arm-linux-gnueabihf-2012.07-20120720_linux/bin:$PATH
export ARCH=arm
export CROSS_COMPILE=arm-linux-gnueabihf-
make cedric_config
make clean
make 2>&1 | tee abc.log
