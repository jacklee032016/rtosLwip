# /bin/bash

PWD=`pwd`

echo $PWD

LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$PWD/Linux.bin.X86/lib
export LD_LIBRARY_PATH

PATH=$PWD/Linux.bin.X86/usr/bin:$PATH
export PATH

echo $LD_LIBRARY_PATH
echo $PATH

