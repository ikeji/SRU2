# Nonterminal symbol
symbol :program, :expression, :list, :define, :lamb, :call, :ref, :literal
# Terminal symbol
symbol :spc, :id, :stringliteral, :numericliteral

manipulator :prog_begin, :prog_rep, :prog_end
program <= prog_begin * r(expression * prog_rep(:prog_begin,:expression)) * prog_end(:prog_begin)

expression <= spc * ( list | ref | literal )

list <= spc * "(" * (define | lamb | call ) * ")"

manipulator :def_creat
define <= spc * "define" * spc * id * spc * expression * def_creat(:id, :expression)

manipulator :lamb_begin, :lamb_arg, :lamb_end
lamb <= spc * "lambda" * lamb_begin * spc * "(" *
  spc * id * lamb_arg(:lamb_begin,:id) * r( spc * "," *spc * id * lamb_arg(:lamb_begin,:id) ) * spc * ")" *
  program * lamb_end(:lamb_begin, :program)

manipulator :call_begin, :call_rep, :call_end
call <= expression * call_begin(:expression) *
  r(spc * expression * call_rep(:call_begin, :expression)) *
  call_end(:call_begin)

manipulator :ref_ins
ref <= spc * id * '.' * ref * ref_ins(:id,:ref) | id 

literal <= spc * (stringliteral | numericliteral)

