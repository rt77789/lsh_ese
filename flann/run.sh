#!/bin/bash


export LD_LIBRARY_PATH=`pwd`/lib:${LD_LIBRARY_PATH}
echo ${LD_LIBRARY_PATH}

./a.out $1 $2
