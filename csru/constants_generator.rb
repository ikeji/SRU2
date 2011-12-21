constants_data = <<EOL
# Default fields
self
class,klass
local
_parent
__name
superclass
instanceMethods
findInstanceMethod
__findSlot
initialize
#
currentStackFrame
findSlotMethod
subclass
toS
#
$$,doldol
$$2,doldol2
$$3,doldol3
_
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
Math
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
percent
invert
times
_i
_j
_block
_times_internal
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
apply
ylppa
proc
# Object
new,mew
# for __parser
src
ast
pos
status
trueResult
falseResult
program
error
memoize
clearMemoize
# for syntax
return,leturn
# for basic library
require
requireNative
load
eval
p
puts
print
# for Array
at
slice
assoc
choice
clear
collect!,collectEx
combination
compact
compact!,compactEx
concat
delete,derete
deleteIf,dereteIf
each
eachIndex
empty?,emptyQ
fetch
fill
flatten
flatten!,flattenEx
index
insert
join
last
length
map!,mapEx
nitems
pack
permutation
pop
product
push
rassoc
reject!,rejectEx
replace
reverse
reverse!,reverseEx
rindex
shift
shuffle
shuffle!,shuffleEx
size
slice!,sliceEx
sort!,sortEx
toA
transpose
uniq
uniq!,uniqEx
unshift
valuesAt
_a
_each_internal
# Math
sin
cos
srand
rand
# Sys
Sys
clock
# String
substr
# AST
ast
AST
LetExpression
RefExpression
CallExpression
ProcExpression
StringExpression
# binding
getLocalMethod
setLocalMethod
EOL

200.times do |i|
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
