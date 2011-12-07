#!/usr/bin/bash

TOPK=3

if [[ $1 = "-test" ]]
then
#	make
	echo "Using test dataset..."
	#./test -build dataset.input dataset.index
	#./test -load dataset.input dataset.index dataset.test 10
#	./test -build ${2} ${2}.index
	./test -load ${2} ${2}.index ${3} ${TOPK}
elif [[ $1 = "-mpl" ]]
then
	echo "mplshash test dataset..."
	# index & dataset.input load from config's path, not ${2}.
	./test -mpl ${2} ${2}.index ${3} ${TOPK}
elif [[ $1 = "-bi" ]]
then
	echo "Buildint index..."
	./test -build ${2} ${2}.index
elif [[ $1 = "-nf" ]]
then
#	make
	echo "Using naive FFT test..."
	./test -nf ${2} ${3} ${TOPK}
	#./test -nf dataset.input dataset.test 10
elif [[ $1 = "-nw" ]]
then
#make
	echo "Using naive FFT & Wavelet test..."
	./test -nw ${2} ${3} ${TOPK}
	#./test -nw dataset.input dataset.test 10
elif [[ $1 = "-build" ]]
then
	#perl build_data.pl < filt3.wavelet.3.0 >  dataset.test.raw
	./main -trans ${2} ${2}.input
	./mplshash/txt2bin ${2} mplshash/${2}.mpl.input
else
	echo -e "usage: \n\
		-test : binary_input_file binary_test_file # use dataset.test for basic lsh testing.\n\
		-mpl: binary_input_file binary_test_file # use dataset.test for mpl testing.\n\
		-build : raw_file 	#build new dataset.test file.\n\
		-bi:	build index file.\n\
		-nf	: binary_input_file binary_test_file # naive FFT-Convolution computing cross-correlation.\n\
		-nw	: binary_input_file binary_test_file # naive wavelet transform and FFT-Convolution.\n"
fi
