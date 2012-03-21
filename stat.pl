#!/usr/bin/perl -w

sub statis {
	my @res;

	while(<>) {
		chomp;
		my $recall = $1 if m{recall:\s*([^\s]+)}is;
		my $cost = $1 if m{cost:\s*([^\s]+)}is;

		#print $recall, " ", $cost, "\n";
		push @res, [$recall, $cost];
	}

	for (sort { $a->[0] < $b->[0] ? -1 : $a->[0] > $b->[0] ? 1 : $a->[1] <=> $b->[1] } @res) {
		print $_->[0], " ", $_->[1], "\n";
	}
}

sub id2name {
	#my $sac_path = 'data/million_data/';
	my $sac_path = 'wenhuan/wenhuan/';
	#my $map_path = 'wavelet/million.map';
	my $map_path = 'wavelet/enh.map';

	open MAP, "<$map_path" or die "open $map_path fail.";
	my %map;
	while(<MAP>) {
		chomp;
		my ($id, $name) = split /\s+/;
		$map{$id} = $name;
	}
	while(<>) {
		chomp;
		my @tokens = split /\s+/;
		unless($map{$tokens[0]}) {
			warn "$tokens[0] is undef\n";
			print "NULL\n";
			next;
		}
		#die "$tokens[0] undef\n" unless $map{$tokens[0]};
		#print $#tokens, "\n";
		# read sac and get b.
		#my $delta = `wavelet/pssac/read_delta $sac_path/$map{$tokens[0]}`;
		#print "$map{$tokens[0]} $delta\n";
		print "$map{$tokens[0]}\n";
	}
}

&id2name();
