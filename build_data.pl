#!/usr/bin/perl -w

#
# perl build_data.pl < filt3.wavelet.3.0 >  dataset.test.raw
#

my $i = 0;
while(<>) {
	if($i % 4 == 3) {
		chomp;
		my @a = map { $_} split /\s+/;
		
		shift @a;
		print join(" ", @a), "\n";
	}
	++$i;
}
