#!/usr/bin/bash


if [[ $1 = "-test" ]]
then
	echo "Using test dataset..."
	make
	./test -build dataset.input dataset.index
	./test -load dataset.input dataset.index dataset.test 10
elif [[ $1 = "-naive" ]]
then
	echo "Using naive test..."
	make
	./test -naive dataset.input dataset.test 10
elif [[ $1 = "-rand" ]]
then
	echo "Using input dataset..."
	make
	./test -build dataset.rand.10000 dataset.index.10000
	./test -load dataset.rand.10000 dataset.index.10000 dataset.rand.10000 10
elif [[ $1 = "-bt" ]]
then
	make
	perl build_data.pl < filt3.wavelet.3.0.250 >  dataset.test.raw
	./main -trans dataset.test.raw dataset.test
elif [[ $1 = "-bi" ]]
then
	make
	perl build_data.pl < filt3.wavelet.3.0.250 >  dataset.raw
	./main -trans dataset.raw dataset.input

elif [[ $1 = "-build" ]]
then
	make
	./test -build dataset.input dataset.index
else
	echo -e "usage: \n\
		-test : use dataset.test for testing.\n\
		-rand : use rand.10000 for testing.\n\
		-build: build index of the dataset.input.\n\
		-bi: build new dataset.input file.\n\
		-bt: build new dataset.test file.\n\
		-naive : naive wavelet transforming test."
fi


