#!/usr/bin/bash

if [[ $1 = "-test" ]]
then
#	make
	echo "Using test dataset..."
	#./test -build dataset.input dataset.index
	#./test -load dataset.input dataset.index dataset.test 10
#	./test -build ${2} ${2}.index
	./test -load ${2} ${2}.index ${3} 1

elif [[ $1 = "-nf" ]]
then
#	make
	echo "Using naive FFT test..."
	./test -nf ${2} ${3} 1
	#./test -nf dataset.input dataset.test 10
elif [[ $1 = "-nw" ]]
then
#make
	echo "Using naive FFT & Wavelet test..."
	./test -nw ${2} ${3} 1
	#./test -nw dataset.input dataset.test 10
elif [[ $1 = "-build" ]]
then
	perl build_data.pl < filt3.wavelet.3.0 >  dataset.test.raw
	./main -trans dataset.test.raw dataset.test 
else
	echo -e "usage: \n\
		-test : binary_input_file binary_test_file # use dataset.test for testing.\n\
		-build : build new dataset.test file.\n\
		-nf	: binary_input_file binary_test_file # naive FFT-Convolution computing cross-correlation.\n\
		-nw	: binary_input_file binary_test_file # naive wavelet transform and FFT-Convolution.\n"
fi
