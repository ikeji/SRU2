require "yapp"

p = ParserBuilder.build() do
  symbol :program, :expression, :list, :spc, :define, :lambda, :call, :id,
         :ref, :stringliteral, :literal
  manipulator :begin_repeat, :repeater, :end_repeat
  program <= begin_repeat * r(expression * repeater(:begin_repeat,:expression)) * end_repeat(:begin_repeat)
  expression <= spc * ( list | ref | literal )
  list <= spc * "(" * (define | lambda | call ) * ")"
  define <= spc * "define" * spc * id * spc * list
  lambda <= spc * "lambda" * spc * "(" * spc * id * r( spc * "," *spc * id ) * spc * ")" * program
  call <= expression * r(spc * expression)
  ref <= spc * id * '.' * ref | id 
  literal <= spc * stringliteral
end

#puts Printer.to_s(p)
puts CppCodeBuilder.to_s(p)

# vim:sw=2:ts=1000
