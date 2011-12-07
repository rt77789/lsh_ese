#!/bin/bash

/usr/bin/c++    -I/home/rt777789/code/project/lshkit/lshkit-0.2.1/include -I/usr/local/include  -o ${1}.o -c ${1}

/usr/bin/c++     ${1}.o  -o ${1}.run -rdynamic ../lib/liblshkit.a -Wl,-Bstatic -lboost_program_options-mt -Wl,-Bdynamic -L/usr/local/lib -lgsl -lgslcblas -lm



