#!/usr/bin/perl -w

#
# perl build_data.pl < filt3.wavelet.3.0 >  dataset.test.raw
#

sub filt3 {
	my $i = 0;
	while(<>) {
		if($i % 4 == 3) {
			chomp;
			my @a = map { $_ - 2} split /\s+/;

			shift @a;
			print join(" ", @a), "\n";
		}
		++$i;
	}
}

sub million_cut {
	my $i = 0;
	my $rows = 1310904;
	my $size = 109242;

	for my $num (0..($rows / $size - 1)) {
		open OUT, ">million_data.raw.$num" or die "open million_data.raw.$num fail...";
		for (1..$size) {
			my $line = <>;
			print OUT $line;
			++$i;
		}
		close OUT;
	}
	print STDERR $i, "\n";
}

sub million_trans {
	for my $file (`ls million_data.raw.*`) {
		chomp($file);
		if($file =~ m{^million_data\.raw\.(\d+)$}is) {
			system("./main -trans $file million_data.raw.input.$1");
		}
	}
}

&million_trans();
#&million_cut();
