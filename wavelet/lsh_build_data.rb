#!/usr/bin/ruby

i=0
STDIN.readlines.each do |line|
	if(i % 4 == 3)
		tokens = line.split(/\s/)
		tokens.shift
		puts tokens.join(" ")
	end
	i += 1
end
