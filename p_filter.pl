#!/usr/bin/perl -w

while(<stdin>) {
	chomp;
	my @a = split /\s+/;
	my $s = $ARGV[0];
	print join " ", @a[0..($s-1)];
	print "\n";
}
