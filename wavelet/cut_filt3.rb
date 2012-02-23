#!/usr/bin/ruby 

if($0 == __FILE__) 
	lines = [];
	STDIN.readlines.each do |line|
		lines.push(line)
	end
	puts "#{lines.length}"

	0.upto(3).each do |i|
		outf = File.new('filt3.wavelet.3.' + i.to_s, "w")
		start = (lines.length / 4 - 2).to_i
		lines[0, start].each do |a|
			outf.write a
		end
	end
end
