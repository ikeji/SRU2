require "optparse"
require "yapp"

$output = Printer

OptionParser.new do |opt|
  opt.on('-p','--print'){ $output = Printer }
  opt.on('-c','--cpp'){ $output = CppCodeBuilder }
  opt.parse!(ARGV)
end

if(ARGV.size < 1)
  puts "main.rb FILENAME"
  exit
end

p = ParserBuilder.build() do
  eval(File.read(ARGV[0]), binding)
end

puts $output.to_s(p)

# vim:sw=2:ts=1000

