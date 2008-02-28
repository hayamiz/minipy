#!/usr/bin/ruby

input = STDIN.read

rex = /run: [\w-]+\/([\.\/\w]+)\n\nreal\s+(\d+)m([\d\.]+)s/

data = {}

input.gsub(rex) do |match|
  data[$1] = Array.new if ! data.has_key?($1)
  data[$1].push($2.to_i * 60 + $3.to_f)
end

data.sort.each do |record|
  puts "#{record[0]}, #{record[1].inject(0){|ret, x| ret + x / record[1].size}}"
end
