constants_data = <<EOL
# Default fields
self
class,klass
_parent
__name
superclass
instanceMethods
__findSlot
#
currentStackFrame
findSlotMethod
subclass
#
$$,doldol
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
__parser
# for boolean
ifTrue
ifFalse
ifTrueFalse
#  names for boolean.
True
False
# for methods which can overwritten
pipepipe
ampamp
superEqual
greaterOrEqual
lessOrEqual
pipe
amp
ltlt
gtgt
exclamation
tilde
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
invert
# map
get
set
# proc
loop
whileTrue
condition
block
whileTrue_internal
loop_internal
result
break,bleak
next
# Object
new,mew
# for __parser
src
ast
pos
status
trueResult
program
error
memoize
clearMemoize
# for syntax
return,leturn
# for basic library
require
requireNative
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
error#{i}
left#{i}
right#{i}
EOL
end

constants_data += File.read("symbols")

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
#include "symbol.h"
#include <string>

namespace sym {

using namespace std;
    EOL
    constants.each do |sym|
    result += <<-EOL
const sru::symbol& #{sym.name}(){
  static sru::symbol ret("#{sym.text}");
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

namespace sru{
class symbol;
}

namespace sym {

    EOL
    constants.each do |sym|
    result += <<-EOL
const sru::symbol& #{sym.name}();
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
