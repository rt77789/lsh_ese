#!/bin/bash


prefix=/usr/local
exec_prefix=${prefix}
libdir=${prefix}/lib64
includedir=${prefix}/include

libs=/usr/lib64

flann_includedir=./src/cpp/

flann_libdir=./lib/


Libs="-L${libdir} -L${flann_libdir} -L${libs} -lflann -lhdf5"
Cflags="-I${includedir} -I${flann_includedir}"

echo "g++ $1 ${Cflags} ${Libs} "
g++ $1 ${Cflags} ${Libs}

