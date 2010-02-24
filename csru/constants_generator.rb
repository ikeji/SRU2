constants_data = <<EOL
# Default fields
class,klass
parent
name
self
superclass
instanceMethods
findSlot
#
CurrentStackFrame
findSlotMethod
# Classes
nil
Binding
Class
Object
Proc
Array
Hash
String
Numeric
Boolean
true,tlue
false,farse
sru_parser
# for boolean
ifTrue
ifFalse
ifTrueFalse
True
False
# for numeric
parse
equal
notEqual
greaterThan
lessThan
plus
minus
asterisk
slash
# proc
loop
whileTrue
condition
block
whileTrue_internal
loop_internal
result
# for sru_parser
src
ast
pos
status
trueResult
program
# TODO: get from parser_generator
program
expression
list
define
lamb
call
ref
literal
target
spc
id
stringliteral
numericliteral
ref_ins
def_creat
lamb_begin
lamb_arg
lamb_end
call_begin
call_rep
call_end
EOL

100.times do |i|
  constants_data += <<EOL
term#{i}
pos#{i}
status#{i}
result#{i}
last#{i}
block#{i}
break#{i}
EOL
end

require "optparse"

class Sym
  attr :name
  attr :text
  def initialize(line)
    a = line.split(/,/)
    raise Exception.new if a.size == 0
    @text = a[0]
    if(a.size > 1)
      @name = a[1]
    else
      @name = a[0]
    end
  end
end

constants = constants_data.
            split(/\n/).
            delete_if{|l|l=~/^#/}.
            uniq.
            map{|l| Sym.new(l)}

class CppCodeBuilder
  def self.output(constants)
    result = <<-EOL
// Programing Language SRU
// Copyright(C) 2005-2010 IKeJI
//

#include "constants.h"
#include <string>

namespace sym {

using namespace std;
    EOL
    constants.each do |sym|
    result += <<-EOL
const string& #{sym.name}(){
  static string ret = "#{sym.text}";
  return ret;
}
    EOL
    end
    result += <<-EOL

}  // namespace sym
    EOL

    return result
  end
end

class HCodeBuilder
  def self.output(constants)
    result = <<-EOL
// Programing Language SRU
// Copyright(C) 2005-2010 IKeJI
//

#ifndef CONSTANTS_H_
#define CONSTANTS_H_

#include <string>

namespace sym {

    EOL
    constants.each do |sym|
    result += <<-EOL
const std::string& #{sym.name}();
    EOL
    end

    result += <<-EOL

}  // namespace sym

#endif // CONSTANTS_H_
    EOL

    result
  end
end

OptionParser.new do |opt|
  opt.on('-c','--cpp'){ $output = CppCodeBuilder }
  opt.on('-h','--header'){ $output = HCodeBuilder }
  opt.parse!(ARGV)
end

puts $output.output(constants)
