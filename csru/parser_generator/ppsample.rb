require "yapp"

p = ParserBuilder.build() do
  symbol :program, :expression, :list, :spc, :define, :lambda, :call, :id,
         :ref, :stringliteral, :literal
  program <= r(expression)
  expression <= list | ref | literal 
  list <= spc * "(" * (define | lambda | call ) * ")"
  define <= "define" * spc * id * spc * list
  lambda <= "lambda" * spc * "(" * spc * id * r( spc * "," *spc * id ) * spc * ")" * program
  call <= program * r(spc * program)
  ref <= id * '.' * ref | id 
  literal <= stringliteral
end

#puts Printer.to_s(p)
puts CppCodeBuilder.to_s(p)

# vim:sw=2:ts=1000
