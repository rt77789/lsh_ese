#!/usr/bin/bash

if [[ $1 = "-test" ]]
then
	make
	echo "Using test dataset..."
	./test -build dataset.input dataset.index
	./test -load dataset.input dataset.index dataset.test 10
elif [[ $1 = "-nf" ]]
then
	make
	echo "Using naive FFT test..."
	./test -nf dataset.input dataset.test 10
elif [[ $1 = "-nw" ]]
then
make
	echo "Using naive FFT & Wavelet test..."
	./test -nw dataset.input dataset.test 10

elif [[ $1 = "-load" ]]
then
	make
	echo "Using input dataset..."
	./test -build dataset.rand.10000 dataset.index.10000
	./test -load dataset.rand.10000 dataset.index.10000 dataset.rand.10000 10
elif [[ $1 = "-build" ]]
then
	perl build_data.pl < filt3.wavelet.3.0 >  dataset.test.raw
	./main -trans dataset.test.raw dataset.test 
else
	echo -e "usage: \n\
		-test : use dataset.test for testing.\n\
		-rand : use rand.10000 for testing.\n\
		-build : build new dataset.test file.\n\
		-nf	: naive FFT-Convolution computing cross-correlation.\n\
		-nw	: naive wavelet transform and FFT-Convolution.\n"
fi
